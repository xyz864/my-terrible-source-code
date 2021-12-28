"Resource/UI/TeamMenu.res"
{
 "team"
{
 "ControlName"  "CTeamMenu"
 "fieldName"  "team"
 "xpos"  "100"
 "ypos"  "60"
 "wide"  "240"
 "tall"  "290"
 "autoResize"  "0"
 "pinCorner"  "0"
 "visible"  "1"
 "enabled"  "1"
 "tabPosition"  "0"
 "settitlebarvisible"  "0"
 "title"  "Team selection"
}
"joinTeam"
{
 "ControlName"  "Label"
 "fieldName"  "joinTeam"
 "xpos"  "20"
 "ypos"  "20"
 "wide"  "200"
 "tall"  "30"
 "autoResize"  "0"
 "pinCorner"  "0"
 "visible"  "1"
 "enabled"  "1"
 "tabPosition"  "0"
 "labelText"  "Join a team"
 "textAlignment"  "center"
 "dulltext"  "0"
 "brighttext"  "0"
 "font"  "MenuTitle"
 "wrap"  "0"
}
"jointeam1"
{
 "ControlName"  "Button"
 "fieldName"  "jointeam1"
 "xpos"  "20"
 "ypos"  "60"
 "wide"  "200"
 "tall"  "30"
 "autoResize"  "0"
 "pinCorner"  "2"
 "visible"  "1"
 "enabled"  "1"
 "tabPosition"  "3"
 "labelText"  "Join the Greens"
 "textAlignment"  "west"
 "dulltext"  "0"
 "brighttext"  "0"
 "wrap"  "0"
 "Command"  "jointeam 1"
 "Default"  "0"
}
"jointeam2"
{
 "ControlName"  "Button"
 "fieldName"  "jointeam2"
 "xpos"  "20"
 "ypos"  "100"
 "wide"  "200"
 "tall"  "30"
 "autoResize"  "0"
 "pinCorner"  "2"
 "visible"  "1"
 "enabled"  "1"
 "tabPosition"  "4"
 "labelText"  "Join the Greys"
 "textAlignment"  "west"
 "dulltext"  "0"
 "brighttext"  "0"
 "wrap"  "0"
 "Command"  "jointeam 2"
 "Default"  "0"
}

	"jointeam3"
	{
		"ControlName"		"Button"
		"fieldName"		"jointeam3"
		"xpos"			"76"
		"ypos"			"148"
		"wide"			"148"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"2"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"4"
		"labelText"		"#TM_Join_Team_3"
		"textAlignment"		"west"
		"dulltext"		"0"
		"brighttext"		"0"
		"command"		"jointeam 3"
	}	

"autojoin"
{
 "ControlName"  "Button"
 "fieldName"  "autojoin"
 "xpos"  "20"
 "ypos"  "180"
 "wide"  "200"
 "tall"  "30"
 "autoResize"  "0"
 "pinCorner"  "2"
 "visible"  "1"
 "enabled"  "1"
 "tabPosition"  "1"
 "labelText"  "Auto-Join"
 "textAlignment"  "west"
 "dulltext"  "0"
 "brighttext"  "0"
 "wrap"  "0"
 "Command"  "jointeam 0"
 "Default"  "0"
}
"CancelButton"
{
 "ControlName"  "Button"
 "fieldName"  "CancelButton"
 "xpos"  "20"
 "ypos"  "240"
 "wide"  "200"
 "tall"  "30"
 "autoResize"  "0"
 "pinCorner"  "2"
 "visible"  "1"
 "enabled"  "1"
 "tabPosition"  "0"
 "labelText"  "Cancel"
 "textAlignment"  "center"
 "dulltext"  "0"
 "brighttext"  "0"
 "wrap"  "0"
 "Command"  "vguicancel"
 "Default"  "1"
}
}