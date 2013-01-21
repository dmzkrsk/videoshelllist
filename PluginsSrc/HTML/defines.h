#define RET_FORMAT_HEADER	"<html>\r\n<title>Информация о файлах</title>\r\n<style>\r\nbody, table {font-family:arial; font-size:12}\r\n.line0 {background-color:white}\r\n.line1 {background-color:cfcfcf}\r\ntd {text-align:center}\r\nth {background-color:black;color:gold;}\r\n</style>\r\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1251\">\r\n<body>\r\n<table cellspacing=1 cellpadding=2 border=0 bgcolor=black align=center>\r\n";
#define RET_FORMAT_MIDDLE	""
#define RET_FORMAT_END		"</table>\r\n<p align=right>Сгенерировано VideoList</p></body>\r\n</html>";

#define RET_LINE_HEADER		"<tr class=\"line%%PARITY%%\"><td>";
#define RET_LINE_MIDDLE		"</td><td>"
#define RET_LINE_END		"</td></tr>\r\n";

#define RET_TABLE_HEADER	"<tr class=\"header\"><th>";
#define RET_TABLE_MIDDLE	"</th><th>"
#define RET_TABLE_END		"</th></tr>";

#define STR_TITLE		"HTML таблица"
#define STR_EXTENSION		"html"
#define STR_MESSAGE		"HTML таблица создана"
#define STR_STATUS		"HTML файл с подробной информацией о файлах"
