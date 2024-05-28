//
//  HTML PAGE
//
const char Page_PriskirtiDS18B20[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<link rel="stylesheet" type="text/css" href="/style.css">
<div class="smartphone">
  <div class="content">
<center><a href="ds18b20.html"  class="myButton"><</a><span class="textas"> DS18B20 priskyrimas </span>
<a href="ntp.html"  class="myButton">></a>

<hr>
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" >
<tr><td align="center">Rasta</td><td align="center">Priskirta</td></tr>
<tr><td align="left">t 0 : <span id="t0"></span></td><td align="right">Kolektorius: <input type="text" id="Kid" name="Kid" size="2" maxlength="2" value=""></td></tr>
<tr><td align="left">t 1 : <span id="t1"></span></td><td align="right">Boileris: <input type="text" id="Bid" name="Bid" size="2" maxlength="2" value=""></td></tr>
<tr><td align="left">t 2 : <span id="t2"></span></td><td align="right">Oras lauke: <input type="text" id="OLid" name="OLid" size="2" maxlength="2" value=""></td></tr>
<tr><td align="left">t 3 : <span id="t3"></span></td><td align="right">Oras kambary: <input type="text" id="OKid" name="OKid" size="2" maxlength="2" value=""></td></tr>
<tr><td align="left">t 4 : <span id="t4"></span></td><td align="right">Akumuliacinės viršus: <input type="text" id="AVid" name="AVid" size="2" maxlength="2" value=""></td></tr>
<tr><td align="left">t 4 : <span id="t5"></span></td><td align="right">Akumuliacinės apačia: <input type="text" id="AAid" name="AAid" size="2" maxlength="2" value=""></td></tr>
<tr><td align="left">t 4 : <span id="t6"></span></td><td align="right">Pamaišymo vožtuvas: <input type="text" id="PVid" name="PVid" size="2" maxlength="2" value=""></td></tr>
<tr><td align="left">t 5 : <span id="t7"></span></td><td align="right">Katilas: <input type="text" id="KKKid" name="KKKid" size="2" maxlength="2" value=""></td></tr>
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="myButton" value="Įrašyti"></td></tr>
</table>
</form>
<div align="center"><a href="/"   style="width:220px"  class="myButton" >Pradžia</a></div><br><br>

</div></div>
<script>
window.onload = function ()
{ load("microajax.js","js", function() 
    { setValues("/admin/priskirtids18b20values");
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


void send_PriskirtiDS18B20_html()
{
  if (server.args() > 0 )  // Save Settings
  {
    String temp = "";


    //------------------------------------------------------
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      if (server.argName(i) == "Kid") config.Kid = server.arg(i).toInt();
      if (server.argName(i) == "Bid") config.Bid = server.arg(i).toInt(); 
      if (server.argName(i) == "OLid") config.OLid = server.arg(i).toInt(); 
      if (server.argName(i) == "OKid") config.OKid = server.arg(i).toInt(); 
      if (server.argName(i) == "AVid") config.AVid = server.arg(i).toInt(); 
      if (server.argName(i) == "AAid") config.AAid = server.arg(i).toInt(); 
      if (server.argName(i) == "PVid") config.PVid = server.arg(i).toInt(); 
      if (server.argName(i) == "KKKid") config.KKKid = server.arg(i).toInt(); 
    }
    //------------------------------------------------------
    WriteConfig();   
    firstStart = true;
  }
  server.send ( 200, "text/html", Page_PriskirtiDS18B20 ); 
  Serial.println(__FUNCTION__); 
}

void send_PriskirtiDS18B20_values_html()
{ 
  String values ="";
  values += "Kid|" + (String) config.Kid + "|input\n";
  values += "Bid|" + (String) config.Bid + "|input\n";
  values += "OLid|" + (String) config.OLid + "|input\n";
  values += "OKid|" + (String) config.OKid + "|input\n";
  values += "AVid|" + (String) config.AVid + "|input\n";
  values += "AAid|" + (String) config.AAid + "|input\n";
  values += "PVid|" + (String) config.PVid + "|input\n";
  values += "KKKid|" + (String) config.KKKid + "|input\n";
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
