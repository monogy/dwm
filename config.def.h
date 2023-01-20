/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 2;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 0;    /* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/

static const unsigned int gappih    = 2;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 2;       /* vert inner gap between windows */
static const unsigned int gappoh    = 2;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 2;       /* vert outer gap between windows and screen edge */
static       int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */

static const int showsystray        = 1;        /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 0;        /* 0 means bottom bar */
static const Bool viewontag         = True;     /* Switch view on tag switch */
// static const char *fonts[]   = { "SauceCodePro Nerd Font Mono:size=12" , "JoyPixels:pixelsize=12"};
// static const char *fonts[]   = {"SauceCodePro Nerd Font Mono:size=12"};
static const char *fonts[]   = { "SumSun:size=12", "SauceCodePro Nerd Font:size=12" , "JoyPixels:pixelsize=12"};
static const char dmenufont[] = "SauceCodePro Nerd Font Mono:size=12";

// static const char col_gray1[] = "#222222"; //bar背景色
// static const char col_gray2[] = "#333333";
// static const char col_gray3[] = "#bbbbbb"; //bar及未激活窗口tag字体颜色
// static const char col_gray4[] = "#eeeeee"; //仅窗口tag及其name字体颜色
// static const char col_cyan[] = "#37474F";//仅窗口tag及其name背景颜色


static const char col_gray1[] = "#222222"; //bar背景色
static const char col_gray2[] = "#333333";
static const char col_gray3[] = "#bbbbbb"; //bar及未激活窗口tag字体颜色
static const char col_gray4[] = "#00FF00"; //仅窗口tag及其name字体颜色
static const char col_cyan[] = "#37474F";//仅窗口tag及其name背景颜色


static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
	[SchemeHid]  = { col_cyan,  col_gray1, col_cyan  },
};

/* tagging */
// static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
static const char* tags[] = { "一", "二", "三", "四", "五", "六", "七", "八", "九" };

static const unsigned int ulinepad	= 5;	/* horizontal padding between the underline and tag */
static const unsigned int ulinestroke	= 2;	/* thickness / height of the underline */
static const unsigned int ulinevoffset	= 0;	/* how far above the bottom of the bar the line should appear */
static const int ulineall 		= 0;	/* 1 to show underline on all tags, 0 for just the active ones */

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
  { "Vmware",   NULL,       NULL,       1 << 7,       0,           -1 },
  { "wps",      NULL,       NULL,       1 << 6,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
	{ "[@]",      spiral },
	{ "[\\]",     dwindle },
	{ "H[]",      deck },
	{ "TTT",      bstack },
	{ "===",      bstackhoriz },
	{ "HHH",      grid },
	{ "###",      nrowgrid },
	{ "---",      horizgrid },
	{ ":::",      gaplessgrid },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

#define STATUSBAR "dwmblocks"

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "st", NULL };
static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[] = { "st", "-t", scratchpadname, "-g", "120x34", NULL };


static const char* flameshot[] = { "flameshot", "gui", NULL };
// static const char *qalculate[] = {"snap", "run", "qalculate"};
static const char* voltool[] = { "st", "-e", "alsamixer", "m", NULL };
static const char* opentop[] = { "st", "-e", "btop", NULL };
static const char* openranger[] = { "st", "-e", "ranger", NULL };


// static const Key keys[] = {
	// [> modifier                     key        function        argument <]
	// { MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	// { MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	// { MODKEY,                       XK_b,      togglebar,      {0} },
	// { MODKEY,                       XK_j,      focusstackvis,  {.i = +1 } },
	// { MODKEY,                       XK_k,      focusstackvis,  {.i = -1 } },
	// { MODKEY|ShiftMask,             XK_j,      focusstackhid,  {.i = +1 } },
	// { MODKEY|ShiftMask,             XK_k,      focusstackhid,  {.i = -1 } },
	// { MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	// { MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	// { MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	// { MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	// { MODKEY,                       XK_Return, zoom,           {0} },
	// { MODKEY,                       XK_Tab,    view,           {0} },
	// { MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	// { MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	// { MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	// { MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	// { MODKEY,                       XK_space,  setlayout,      {0} },
	// { MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	// { MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	// { MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	// { MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	// { MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	// { MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	// { MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	// { MODKEY,                       XK_s,      show,           {0} },
	// { MODKEY|ShiftMask,             XK_s,      showall,        {0} },
	// { MODKEY,                       XK_h,      hide,           {0} },
	// TAGKEYS(                        XK_1,                      0)
	// TAGKEYS(                        XK_2,                      1)
	// TAGKEYS(                        XK_3,                      2)
	// TAGKEYS(                        XK_4,                      3)
	// TAGKEYS(                        XK_5,                      4)
	// TAGKEYS(                        XK_6,                      5)
	// TAGKEYS(                        XK_7,                      6)
	// TAGKEYS(                        XK_8,                      7)
	// TAGKEYS(                        XK_9,                      8)
	// { MODKEY|ShiftMask,             XK_q,      quit,           {0} },
// };
//


static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
  { MODKEY,                       XK_grave,  togglescratch,  {.v = scratchpadcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },

  { MODKEY,                       XK_j,      focusstackvis,  {.i = +1 } },
	{ MODKEY,                       XK_l,      focusstackvis,  {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_j,      focusstackhid,  {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_l,      focusstackhid,  {.i = -1 } },
  { MODKEY,                       XK_i,      focusstackvis,  {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstackvis,  {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_i,      focusstackhid,  {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,      focusstackhid,  {.i = -1 } },

	{ MODKEY,                       XK_e,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
  
	{ MODKEY,                       XK_Left,   setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_Right,  setmfact,       {.f = +0.05} },

  { MODKEY|Mod4Mask,              XK_h,      incrgaps,       {.i = +1 } },
  { MODKEY|Mod4Mask,              XK_l,      incrgaps,       {.i = -1 } },
  { MODKEY|Mod4Mask|ShiftMask,    XK_h,      incrogaps,      {.i = +1 } },
  { MODKEY|Mod4Mask|ShiftMask,    XK_l,      incrogaps,      {.i = -1 } },
  { MODKEY|Mod4Mask|ControlMask,  XK_h,      incrigaps,      {.i = +1 } },
  { MODKEY|Mod4Mask|ControlMask,  XK_l,      incrigaps,      {.i = -1 } },
  { MODKEY|Mod4Mask,              XK_0,      togglegaps,     {0} },
  { MODKEY|Mod4Mask|ShiftMask,    XK_0,      defaultgaps,    {0} },
  { MODKEY,                       XK_y,      incrihgaps,     {.i = +1 } },
  { MODKEY,                       XK_o,      incrihgaps,     {.i = -1 } },
  { MODKEY|ControlMask,           XK_y,      incrivgaps,     {.i = +1 } },
  { MODKEY|ControlMask,           XK_o,      incrivgaps,     {.i = -1 } },
  { MODKEY|Mod4Mask,              XK_y,      incrohgaps,     {.i = +1 } },
  { MODKEY|Mod4Mask,              XK_o,      incrohgaps,     {.i = -1 } },
  { MODKEY|ShiftMask,             XK_y,      incrovgaps,     {.i = +1 } },
  { MODKEY|ShiftMask,             XK_o,      incrovgaps,     {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
  { MODKEY|ShiftMask,             XK_f,      fullscreen,     {0} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_s,      show,           {0} },
	{ MODKEY,                       XK_h,      hide,           {0} },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
	{ MODKEY|ControlMask|ShiftMask, XK_q,      quit,           {1} }, 

  { MODKEY|Mod1Mask,              XK_w,      spawn,          SHCMD("notify-send '📰 正在启动WPS office...\n   将在标签7显示'; wps") },
  { MODKEY|Mod1Mask,              XK_s,      spawn,          SHCMD("notify-send '正在启动密码管理器...'; Password_Manager") },
  { MODKEY|Mod1Mask,              XK_f,      spawn,          SHCMD("notify-send '🌎 正在启动浏览器Firefox...\n   将在标签9显示'; firefox-esr") },
  { MODKEY|Mod1Mask,              XK_v,      spawn,          SHCMD("notify-send '正在启动VmWare Workstation...\n将在标签8显示'; primusrun vmware") },
  { MODKEY|Mod1Mask,              XK_n,      spawn,          SHCMD("notify-send '正在启动XMind...'; optirun xmind") },
  { MODKEY|Mod1Mask,              XK_x,      spawn,          SHCMD("notify-send '正在启动喜马拉雅...'; /opt/apps/com.xmly.deepin/files/run.sh") },
  { MODKEY|Mod1Mask,              XK_b,      spawn,          SHCMD("notify-send '正在启动百度云盘...'; baidunetdisk") },
  { MODKEY|Mod1Mask,              XK_c,      spawn,          SHCMD("notify-send '🍀 正在启动微信...'; weixin") },
  { MODKEY|Mod1Mask,              XK_q,      spawn,          SHCMD("notify-send '🐧 正在启动QQ...'; qq") },
  { MODKEY|Mod1Mask,              XK_m,      spawn,          SHCMD("notify-send '📮 正在启动邮箱...'; thunderbird") },
  { MODKEY|Mod1Mask,              XK_d,      spawn,          SHCMD("/home/monogy/.dotfiles/scripts/toExcute/fileu-toggle.sh") },
  { MODKEY|Mod1Mask,              XK_t,      spawn,          SHCMD("/home/monogy/project/shell/translate.sh") },
  // { MODKEY|Mod1Mask,              XK_p,      spawn,          SHCMD("notify-send '📜 正在启动福昕阅读器...'; FoxitReader") },
  // { MODKEY | Mod1Mask, XK_p, spawn, SHCMD("notify-send '📜 正在启动PDF阅读器...'; masterpdfeditor5") },
  // { MODKEY|Mod1Mask,              XK_p,      spawn,          SHCMD("notify-send '📜 正在启动PDF阅读器...'; PDFstdio") },
  { MODKEY|Mod1Mask,              XK_n,      spawn,          SHCMD("notify-send '🎵 正在启动网易云音乐...'; netease-cloud-music") },
  { MODKEY|Mod1Mask,              XK_l,      spawn,          SHCMD("notify-send '🎵 正在启动洛雪音乐...'; lx-music-desktop") },
  { MODKEY|Mod1Mask,              XK_y,      spawn,          SHCMD("notify-send '📕 正在启动有道词典...'; youdao-dict") },
  { MODKEY|Mod1Mask,              XK_z,      spawn,          SHCMD("notify-send '📺 正在启动ZY-视频播放器...'; ZY-Player") },
  // { MODKEY|ShiftMask,             XK_t,      spawn,          SHCMD("/home/monogy/scripts/tray-toggle.sh") },
  { MODKEY,                       XK_F2,     spawn,          SHCMD("/home/monogy/.dotfiles/scripts/toExcute/reWifi.sh") },
  { MODKEY|ShiftMask,             XK_F2,     spawn,          SHCMD("/home/monogy/.dotfiles/scripts/toExcute/reDhcp.sh") },
  { MODKEY,                       XK_F7,     spawn,          SHCMD("st -e /home/monogy/.dotfiles/dwmbar-functions/power_manager.sh") },
  { MODKEY,                       XK_F8,     spawn,          SHCMD("/home/monogy/.dotfiles/scripts/toBin/screen_extend.sh") },
  { MODKEY,                       XK_F9,     spawn,          SHCMD("/home/monogy/.dotfiles/scripts/toExcute/touchpd_cmd.sh") },
  { MODKEY,                       XK_F10,    spawn,          SHCMD("pkill -RTMIN+19 dwmblocks") },
  // { MODKEY,                       XK_s,      spawn,          SHCMD("gnome-control-center") },
  // { MODKEY,                       XK_t,      spawn,          SHCMD("gnome-system-monitor") },
  { 0,            XF86XK_Calculator,         spawn,          SHCMD("notify-send '正在启动计算器...'; gnome-calculator") },
  { 0,            XF86XK_MonBrightnessUp,    spawn,          SHCMD("brightnessctl -q specific +5%; pkill -RTMIN+8 dwmblocks") },
  { 0,            XF86XK_MonBrightnessDown,  spawn,          SHCMD("brightnessctl -q specific 5%-; pkill -RTMIN+8 dwmblocks") },
  { 0,            XF86XK_AudioMute,          spawn,          SHCMD("/usr/bin/amixer set Master toggle; pkill -RTMIN+10 dwmblocks") },
  { 0,            XF86XK_AudioLowerVolume,   spawn,          SHCMD("/usr/bin/amixer -qM set Master 2%- umute; pkill -RTMIN+10 dwmblocks") },
  { 0,            XF86XK_AudioRaiseVolume,   spawn,          SHCMD("/usr/bin/amixer -qM set Master 2%+ umute; pkill -RTMIN+10 dwmblocks") },
  { 0,            XF86XK_ScreenSaver,        spawn,          SHCMD("/home/monogy/.dotfiles/scripts/toExcute/slock_screen.sh") },
  // { 0,            XF86XK_ScreenSaver,        spawn,          SHCMD("slock & xset dpms force off") },
  // { MODKEY|Mod1Mask,              XK_Print,  spawn,          SHCMD("gnome-screenshot") },
  { MODKEY,                       XK_t,      spawn,          SHCMD("/home/monogy/.dotfiles/scripts/toExcute/tray-toggle.sh") },
  { MODKEY|Mod1Mask,              XK_r,      spawn,          { .v = openranger } },
  { MODKEY,                       XK_Print,  spawn,          { .v = flameshot } },
  { MODKEY,                       XK_Pause,  spawn,          {.v = opentop } },
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
  { MODKEY|Mod1Mask,                       XK_0,      spawn,          SHCMD("WriteIniFile '/home/monogy/.dotfiles/.global_data.ini' 'wallpaper' 'num' '0'; WriteIniFile '/home/monogy/.dotfiles/.global_data.ini' 'wallpaper' 'update' 'ON'") },
  { MODKEY|Mod1Mask,                       XK_1,      spawn,          SHCMD("WriteIniFile '/home/monogy/.dotfiles/.global_data.ini' 'wallpaper' 'num' '1'; WriteIniFile '/home/monogy/.dotfiles/.global_data.ini' 'wallpaper' 'update' 'ON'") },
  { MODKEY|Mod1Mask,                       XK_2,      spawn,          SHCMD("WriteIniFile '/home/monogy/.dotfiles/.global_data.ini' 'wallpaper' 'num' '2'; WriteIniFile '/home/monogy/.dotfiles/.global_data.ini' 'wallpaper' 'update' 'ON'") },
  { MODKEY|Mod1Mask,                       XK_3,      spawn,          SHCMD("WriteIniFile '/home/monogy/.dotfiles/.global_data.ini' 'wallpaper' 'num' '3'; WriteIniFile '/home/monogy/.dotfiles/.global_data.ini' 'wallpaper' 'update' 'ON'") },

};



/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
  // { ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
  // { ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
	{ ClkWinTitle,          0,              Button1,        togglewin,      {0} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button1,        sigstatusbar,   {.i = 1} },
	{ ClkStatusText,        0,              Button2,        sigstatusbar,   {.i = 2} },
	{ ClkStatusText,        0,              Button3,        sigstatusbar,   {.i = 3} },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

