#define FORCE_VSPLIT                                                           \
    1 /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"
#include <X11/XF86keysym.h>

static int showsystray = 1;       /* 是否显示托盘栏 */
static const int newismaster = 0; /* 定义新窗口在栈顶还是栈底 */
static const unsigned int borderpx = 1; /* 窗口边框大小 */
static const unsigned int systraypinning =
    1; /* 托盘跟随的显示器 0代表不指定显示器 */
static const int systraypinningfailfirst =
    1; /* 托盘跟随的显示器 0代表上个聚焦的显示器 1代表当前聚焦的显示器 */
static const unsigned int systrayspacing = 1; /* 托盘间距 */
static const int showbar = 1;                 /* 是否显示状态栏 */
static const int topbar = 1;         /* 指定状态栏位置 0底部 1顶部 */
static const float mfact = 0.6;      /* 主工作区 大小比例 */
static const int nmaster = 1;        /* 主工作区 窗口数量 */
static const int resizef = 20;       /* 动画帧数 */
static const int resizet = 50000;    /* 动画时长 */
static const unsigned int snap = 10; /* 边缘依附宽度 */
static const unsigned int baralpha = 0xc0;    /* 状态栏透明度 */
static const unsigned int borderalpha = 0xdd; /* 边框透明度 */
static const int resizehints = 1;
static const char *fonts[] = {"Iosevka Nerd Font: style=medium:size=12",
                              "feather:size = 12"};

static const char *colors[][3] = {
    [SchemeNorm] = {"#bbbbbb", "#333333", "#444444"},
    [SchemeSel] = {"#ffffff", "#37474F", "#42A5F5"},
    [SchemeHid] = {"#dddddd", "#333333", "#42A5F5"},
    [SchemeSystray] = {"#7799AA", "#7799AA", "#7799AA"}};
static const unsigned int alphas[][3] = {
    [SchemeNorm] = {OPAQUE, baralpha, borderalpha},
    [SchemeSel] = {OPAQUE, baralpha, borderalpha}};

/* 自定义布局 */
static const Layout layouts[] = {
    {"󰙀", tile}, /* 主次栈 */
    {"󰀻", grid}, /* 网格   */
    {"󰕴", dwindle},
    {"", deck},
    {"󰹫", bstack},
    {"󰕭", gaplessgrid},
    {"󰅣", centeredfloatingmaster},
    {"===", bstackhoriz},
    {"###", nrowgrid},
    {"---", horizgrid},
    {"|M|", centeredmaster},
    {NULL, NULL},
};

/* 自定义tag名称 */
/* 自定义特定实例的显示状态 */
static const char *tags[] = {"",  "󰎧", "󰎪", "󰎭", "",  "",
                             "",  "",  "󰎼", "",  "󰎆", "",
                             "󰘅", "󰘑", "",  "",  "",  ""};
static const char *overviewtag = "OVERVIEW";
static const Layout overviewlayout = {"", overview};

static const Rule rules[] = {
    /* class                instance         title       tags mask isfloating
       isfullscreen  monitor */
    {"yesplaymusic", NULL, NULL, 1 << 10, 1, 0, -1},
    {"Postman", NULL, NULL, 1 << 11, 0, 0, -1},
    {"electronic-wechat", NULL, NULL, 1 << 13, 1, 0, -1},
    {NULL, NULL, "broken", 0, 1, 0, -1},
    {NULL, NULL, "图片查看", 0, 1, 0, -1},
    {NULL, NULL, "图片预览", 0, 1, 0, -1},
    {"rdesktop", NULL, NULL, 1 << 8, 1, 0, -1},
    {"float", NULL, NULL, 0, 1, 0, -1},
    {"listen1", NULL, NULL, 0, 1, 0, -1},
    {"icalingua", NULL, NULL, 0, 1, 0, -1},
    {"Gpick", NULL, NULL, 0, 1, 0, -1},
    {"Telegram", NULL, NULL, 1 << 17, 1, 0, -1},
    {"Thunderbird", NULL, NULL, 0, 1, 0, -1},
    {"listen1", NULL, NULL, 1 << 10, 1, 0, -1},
    {"Vncviewer", NULL, NULL, 1 << 4, 1, 0, -1},
    {"pcmanfm", NULL, NULL, 1 << 11, 1, 0, -1},
    {"qq.exe", NULL, NULL, 1 << 12, 1, 0, -1},
    {"tim.exe", NULL, NULL, 1 << 12, 1, 0, -1},
    {"icalingua", NULL, NULL, 1 << 12, 0, 0, -1},
    {"DBeaver", NULL, NULL, 1 << 15, 0, 0, -1},
    {"feishu", NULL, NULL, 1 << 16, 1, 0, -1},
};

// Commands
static const char *upvol[] = {"/home/allen/scripts/vol-up.sh", NULL};
static const char *downvol[] = {"/home/allen/scripts/vol-down.sh", NULL};
static const char *mutevol[] = {"/home/allen/scripts/vol-toggle.sh", NULL};

#define SHCMD(cmd)                                                             \
    {                                                                          \
        .v = (const char *[]) { "/bin/sh", "-c", cmd, NULL }                   \
    }
#define MODKEY Mod4Mask
#define TAGKEYS(KEY, TAG, cmd1, cmd2)                                          \
    {MODKEY, KEY, view, {.ui = 1 << TAG, .v = cmd1}},                          \
        {MODKEY | ShiftMask, KEY, tag, {.ui = 1 << TAG, .v = cmd2}},           \
        {MODKEY | ControlMask, KEY, toggleview, {.ui = 1 << TAG}},

static const char *scratchpadcmd[] = {"st",     "-t", "scratchpad", "-g",
                                      "120x40", NULL}; // 临时小窗口的启动命令

static Key keys[] = {
    /* modifier            key                      function           argument
     */
    {MODKEY,
     XK_minus,
     togglescratch,
     {.v = scratchpadcmd}}, /* super -            |  打开临时小窗 st */
    {MODKEY,
     XK_equal,
     togglesystray,
     {0}}, /* super +            |  切换 托盘栏显示状态 */
    {MODKEY | ShiftMask,
     XK_Tab,
     toggleoverview,
     {0}}, /* super +            |  切换 overview */

    {MODKEY,
     XK_Tab,
     focusstack,
     {0}}, /* super tab          |  本tag内切换聚焦窗口 */
    {MODKEY,
     XK_Up,
     focusstack,
     {.i = -1}}, /* super up           |  本tag内切换聚焦窗口 */
    {MODKEY,
     XK_Down,
     focusstack,
     {0}}, /* super down         |  本tag内切换聚焦窗口 */
    {MODKEY,
     XK_Left,
     viewtoleft,
     {0}}, /* super left         |  聚焦到左边的tag */
    {MODKEY,
     XK_Right,
     viewtoright,
     {0}}, /* super right        |  聚焦到右边的tag */
    {MODKEY | ShiftMask,
     XK_Left,
     tagtoleft,
     {0}}, /* super shift left   |  将本窗口移动到左边tag */
    {MODKEY | ShiftMask,
     XK_Right,
     tagtoright,
     {0}}, /* super shift right  |  将本窗口移动到右边tag */

    {MODKEY | ControlMask,
     XK_j,
     viewtoleft,
     {0}}, /* super contrl j     |  聚焦到左边的tag */
    {MODKEY | ControlMask,
     XK_k,
     viewtoright,
     {0}}, /* super contrl k     |  聚焦到右边的tag */
    {MODKEY | ShiftMask,
     XK_j,
     tagtoleft,
     {0}}, /* super shift j      |  将本窗口移动到左边tag */
    {MODKEY | ShiftMask,
     XK_k,
     tagtoright,
     {0}}, /* super shift k      |  将本窗口移动到右边tag */
    {MODKEY,
     XK_j,
     focusstack,
     {.i = -1}}, /* super j            |  本tag内切换聚焦窗口 */
    {MODKEY,
     XK_k,
     focusstack,
     {0}}, /* super k            |  本tag内切换聚焦窗口 */

    {MODKEY,
     XK_comma,
     setmfact,
     {.f = -0.05}}, /* super ,            |  缩小主工作区 */
    {MODKEY,
     XK_period,
     setmfact,
     {.f = +0.05}}, /* super .            |  放大主工作区 */

    {MODKEY, XK_h, hidewin, {0}}, /* super h            |  隐藏 窗口 */
    {MODKEY | ShiftMask,
     XK_h,
     restorewin,
     {0}}, /* super shift h      |  取消隐藏 窗口 */

    {MODKEY, XK_0, view, {.ui = ~0}}, /*super 0             |  显示所有窗口*/

    {MODKEY | ShiftMask,
     XK_Return,
     zoom,
     {0}}, /* super shift enter  |  将当前聚焦窗口置为主窗口 */
    {MODKEY,
     XK_t,
     togglefloating,
     {0}}, /* super t            |  开启/关闭 聚焦目标的float模式 */
    {MODKEY | ShiftMask,
     XK_t,
     toggleallfloating,
     {0}}, /* super shift t      |  开启/关闭 全部目标的float模式 */
    {MODKEY, XK_f, fullscreen, {0}}, /* super f            |  开启/关闭 全屏 */
    {MODKEY | ShiftMask,
     XK_f,
     togglebar,
     {0}}, /* super shift f      |  开启/关闭 状态栏 */
    {MODKEY,
     XK_e,
     incnmaster,
     {.i = +1}}, /* super e            |  改变主工作区窗口数量 (1 2中切换) */

    {MODKEY,
     XK_s,
     focusmon,
     {.i = +1}}, /* super s            |  光标移动到另一个显示器 */
    {MODKEY | ShiftMask,
     XK_s,
     tagmon,
     {.i = +1}}, /* super shift s      |  将聚焦窗口移动到另一个显示器 */

    {MODKEY | ShiftMask,
     XK_q,
     killclient,
     {0}}, /* super shiift q     |  kill窗口 */
    {MODKEY | ControlMask,
     XK_F12,
     quit,
     {0}}, /* super ctrl f12     |  退出dwm */

    {MODKEY,
     XK_space,
     toggleallhidewins,
     {0}}, /* super space        |  隐藏全部其他窗口 | 显示全部窗口 */

    {MODKEY | ControlMask,
     XK_Up,
     setgap,
     {.i = -6}}, /* super ctrl up      |  窗口增大 */
    {MODKEY | ControlMask,
     XK_Down,
     setgap,
     {.i = +6}}, /* super ctrl down    |  窗口减小 */

    {MODKEY | ShiftMask, XK_0, togglegaps, {0}},
    {MODKEY | ControlMask, XK_space, defaultgaps, {0}},

    {MODKEY | ControlMask, XK_0, setlayout, {.v = &layouts[0]}},
    {MODKEY | ShiftMask,
     XK_space,
     setlayout,
     {.v = &layouts[1]}}, /* super shift space  |  主次栈模式 */
    {MODKEY, XK_r, cyclelayout, {.i = -1}}, /* super r            /  循环布局 */
    {MODKEY | ShiftMask, XK_r, cyclelayout, {.i = +1}},
    {0, XF86XK_AudioLowerVolume, spawn, {.v = downvol}},
    {0, XF86XK_AudioMute, spawn, {.v = mutevol}},
    {0, XF86XK_AudioRaiseVolume, spawn, {.v = upvol}},
    /* spawn + SHCMD 执行对应命令 */
    {0, XK_Print, spawn, SHCMD("~/scripts/app-starter.sh flameshot")},
    {MODKEY, XK_d, spawn, SHCMD("~/scripts/app-starter.sh rofi")},
    {MODKEY | Mod1Mask, XK_k, spawn,
     SHCMD("~/scripts/app-starter.sh screenkey")},
    {MODKEY | ShiftMask, XK_x, spawn, SHCMD("~/scripts/app-starter.sh lock")},
    {MODKEY | ControlMask, XK_t, spawn,
     SHCMD("~/scripts/app-starter.sh clock")},
    {MODKEY, XK_p, spawn, SHCMD("~/scripts/app-starter.sh music")},
    {MODKEY, XK_b, spawn, SHCMD("~/scripts/app-starter.sh browser")},
    {MODKEY | ControlMask, XK_b, spawn,
     SHCMD("~/scripts/app-starter.sh alacritty")},
    {MODKEY, XK_Return, spawn, SHCMD("~/scripts/app-starter.sh term")},
    {MODKEY | ControlMask, XK_Return, spawn,
     SHCMD("~/scripts/app-starter.sh ast")},
    {MODKEY | ShiftMask, XK_y, spawn,
     SHCMD("~/scripts/app-starter.sh clipboard")},
    {MODKEY | ControlMask, XK_y, spawn,
     SHCMD("~/scripts/app-starter.sh notebook")},
    {MODKEY | ControlMask, XK_r, spawn,
     SHCMD("~/scripts/app-starter.sh wallpaper")},
    {Mod1Mask | ControlMask, XK_x, spawn,
     SHCMD("~/scripts/app-starter.sh warpd-h")},
    {Mod1Mask | ControlMask, XK_c, spawn,
     SHCMD("~/scripts/app-starter.sh warpd-n")},
    {Mod1Mask | ControlMask, XK_g, spawn,
     SHCMD("~/scripts/app-starter.sh warpd-g")},
    /* super key : 跳转到对应tag */
    /* super shift key : 将聚焦窗口移动到对应tag */
    /* super ctrl  key : 切换同时显示对应tag */
    /* 若跳转后的tag无窗口且附加了cmd1或者cmd2就执行对应的cmd */
    /* key tag cmd1 cmd2 */
    TAGKEYS(XK_1, 0, 0, 0) TAGKEYS(XK_2, 1, 0, 0) TAGKEYS(
        XK_3, 2, 0, 0) TAGKEYS(XK_4, 3, 0, 0)
        TAGKEYS(XK_5, 4, "~/scripts/app-starter.sh vncviewer", 0) TAGKEYS(
            XK_6, 5, "~/scripts/app-starter.sh pcmanfm",
            0) TAGKEYS(XK_7, 6, "~/scripts/app-starter.sh pycharm",
                       0) TAGKEYS(XK_8, 7, "~/scripts/app-starter.sh idea", 0)
            TAGKEYS(XK_9, 8, "~/scripts/app-starter.sh pavucontrol",
                    0) TAGKEYS(XK_c, 9, "~/scripts/app-starter.sh browser", 0)
                TAGKEYS(XK_p, 10, "~/scripts/app-starter.sh music", 0) TAGKEYS(
                    XK_n, 11, "~/scripts/app-starter.sh postman",
                    0) TAGKEYS(XK_z, 12, "~/scripts/app-starter.sh qq", 0)
                    TAGKEYS(XK_w, 13, "~/scripts/app-starter.sh wechat", 0)
                        TAGKEYS(XK_o, 14, "~/scripts/app-starter.sh virt", 0)
                            TAGKEYS(XK_v, 15, "~/scripts/app-starter.sh db", 0)
                                TAGKEYS(XK_g, 16,
                                        "~/scripts/app-starter.sh feishu", 0)
                                    TAGKEYS(XK_a, 17,
                                            "~/scripts/app-starter.sh telegram",
                                            0)};

static Button buttons[] = {
    /* click               event mask               button            function
       argument  */
    {ClkWinTitle,
     0,
     Button1,
     hideotherwins,
     {0}}, // 左键        |  点击标题  |  隐藏其他窗口仅保留该窗口
    {ClkWinTitle,
     0,
     Button3,
     togglewin,
     {0}}, // 右键        |  点击标题  |  切换窗口显示状态
    {ClkTagBar, 0, Button1, view, {0}}, // 左键        |  点击tag   |  切换tag
    {ClkClientWin,
     MODKEY,
     Button1,
     movemouse,
     {0}}, // super+左键  |  拖拽窗口  |  拖拽窗口
    {ClkClientWin,
     MODKEY,
     Button3,
     resizemouse,
     {0}}, // super+右键  |  拖拽窗口  |  改变窗口大小
    {ClkTagBar,
     MODKEY,
     Button1,
     tag,
     {0}}, // super+左键  |  点击tag   |  将窗口移动到对应tag
};
