
const char PAGE_AkumuliacinesKonfiguracija[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<div class="smartphone">
  <div class="content">
<center><a href="admin.html"  class="myButton"><</a><span class="textas"> Akumuliacinės nustatymai </span>
<a href="pvoztuvas.html"  class="myButton">></a>

<hr>
<form action="" method="get">
<table Atrder="0"  cellspacing="0" cellpadding="3" >
<tr><td align="right">Įjungimo t (°C) :</td><td><input type="text" id="At_ON_T" name="At_ON_T" min="1" max="30" size="2" maxlength="2" value=""></td></tr>
<tr><td align="right">Išjungimo t (°C) :</td><td><input type="text" id="At_OFF_T" name="At_OFF_T" min="1" max="30" size="2" maxlength="2" value=""></td></tr>
<tr><td align="right">Įjungti dabar :</td><td><input type="checkbox" id="At_Rankinis_ijungimas" name="At_Rankinis_ijungimas"></td></tr>
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="myButton" value="Įrašyti"></td></tr>
</table></form>
<div align="center"><a href="/"   style="width:220px"  class="myButton" >Pradžia</a></div>
<br><br>

</div></div>
<script>
  

window.onload = function ()
{
  load("style.css","css", function() 
  {
    load("microajax.js","js", function() 
    {
        setValues("/admin/AkumuliacineValues");
    });
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


void send_AkumuliacinesKonfiguracija_html()
{
  if (server.args() > 0 )  // Save Settings
  {
    config.At_Rankinis_ijungimas = false;
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      if (server.argName(i) == "At_ON_T") config.At_ON_T = server.arg(i).toInt();
      if (server.argName(i) == "At_OFF_T") config.At_OFF_T = server.arg(i).toInt(); 
      if (server.argName(i) == "At_Rankinis_ijungimas") config.At_Rankinis_ijungimas = true; 

    }
    WriteConfig();
    firstStart = true;
  }
  server.send ( 200, "text/html", PAGE_AkumuliacinesKonfiguracija ); 
  Serial.println(__FUNCTION__); 
  
}

void send_AkumuliacinesKonfiguracija_values_html()
{  
  String values ="";
  values += "At_ON_T|" +  (String) config.At_ON_T + "|input\n";
  values += "At_OFF_T|" +  (String) config.At_OFF_T + "|input\n";
  values += "At_Rankinis_ijungimas|" + (String) (config.At_Rankinis_ijungimas ? "checked" : "") + "|chk\n";


  server.send ( 200, "text/plain", values);
  Serial.println(__FUNCTION__);
  Serial.print("At_ON_T : ");Serial.println(config.At_ON_T); 
  Serial.print("At_OFF_T : ");Serial.println(config.At_OFF_T); 
  Serial.print("At_Rankinis_ijungimas : ");Serial.println(config.At_Rankinis_ijungimas); 

  
}
