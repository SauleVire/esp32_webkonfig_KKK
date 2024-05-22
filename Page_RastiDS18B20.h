//
//  HTML PAGE
//
const char Page_RastiDS18B20[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<link rel="stylesheet" type="text/css" href="/style.css">
<div class="smartphone">
  <div class="content">
<center><a href="admin.html"  class="myButton"><</a><span class="textas"> DS18B20 nustatymai </span>
<a href="ntp.html"  class="myButton">></a>

<hr>
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" >
<tr><td align="right">Rasta</td><td>Priskirta</td></tr>
<tr><td align="right">t0 : <span id="t0"></span></td><td>K: <input type="text" id="Kid" name="Kid" size="2" maxlength="2" value=""></td></tr>
<tr><td align="right">t1 : <span id="t1"></span></td><td>B: <input type="text" id="Bid" name="Bid" size="2" maxlength="2" value=""></td></tr>
<tr><td align="right">t2 : <span id="t2"></span></td><td>O: <input type="text" id="Oid" name="Oid" size="2" maxlength="2" value=""></td></tr>
<tr><td align="right">t3 : <span id="t3"></span></td><td>O: <input type="text" id="AVid" name="AVid" size="2" maxlength="2" value=""></td></tr>
<tr><td align="right">t4 : <span id="t4"></span></td><td>O: <input type="text" id="AAid" name="AAid" size="2" maxlength="2" value=""></td></tr>
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="myButton" value="Įrašyti"></td></tr>
</table><br><br>
</form>
<a href="https://saulevire.lt/emoncms9/dashboard/view/testas?apikey=read">sukaupti duomenys</a>
</div></div>
<script>
window.onload = function ()
{ load("microajax.js","js", function() 
    { setValues("/admin/rastids18b20values");
    });
}
function load(e,t,n)
{if("js"==t){
  var a=document.createElement("script");
a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()}
,document.getElementsByTagName("head")[0].appendChild(a)
}
else if("css"==t){
  var a=document.createElement("link");
a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()
}
,document.getElementsByTagName("head")[0].appendChild(a)}
}
</script>
)=====";


void send_RastiDS18B20_html()
{
  if (server.args() > 0 )  // Save Settings
  {
    String temp = "";


    //------------------------------------------------------
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      if (server.argName(i) == "Kid") config.Kid = server.arg(i).toInt();
      if (server.argName(i) == "Bid") config.Bid = server.arg(i).toInt(); 
      if (server.argName(i) == "Oid") config.Oid = server.arg(i).toInt(); 
      if (server.argName(i) == "AVid") config.AVid = server.arg(i).toInt(); 
      if (server.argName(i) == "AAid") config.AAid = server.arg(i).toInt(); 
    }
    //------------------------------------------------------
    WriteConfig();   
    firstStart = true;
  }
  server.send ( 200, "text/html", Page_RastiDS18B20 ); 
  Serial.println(__FUNCTION__); 
}

void send_RastiDS18B20_values_html()
{ 
  String values ="";
  values += "Kid|" + (String) config.Kid + "|input\n";
  values += "Bid|" + (String) config.Bid + "|input\n";
  values += "Oid|" + (String) config.Oid + "|input\n";
  values += "AVid|" + (String) config.AVid + "|input\n";
  values += "AAid|" + (String) config.AAid + "|input\n";
      //------------------------------------------------------

  for(int i=0;i<numberOfDevices;i++){
    tempDev[i] = DS18B20.getTempC( devAddr[i] );
    values += "t" + (String)i +"|" + (String) tempDev[i] + "|div\n";
  }
    DS18B20.requestTemperatures(); //Initiate the temperature measurement

  server.send ( 200, "text/plain", values);
  Serial.println(__FUNCTION__);
//  Serial.print("reiksme1: ");Serial.print(config.reiksme1); 
}
