
const char PAGE_BoilerioKonfiguracija[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<div class="smartphone">
  <div class="content">
<center><a href="admin.html"  class="myButton"><</a><span class="textas"> Boilerio nustatymai </span>
<a href="akumuliacine.html"  class="myButton">></a>

<hr>
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" >
<tr><td align="right">Boilerio įjungimo t (°C) :</td><td><input type="text" id="Bo_ON_T" name="Bo_ON_T" min="1" max="30" size="2" maxlength="2" value=""></td></tr>
<tr><td align="right">Boilerio išjungimo t (°C) :</td><td><input type="text" id="Bo_OFF_T" name="Bo_OFF_T" min="1" max="20" size="2" maxlength="2" value=""></td></tr>
<tr><td align="right">Termostato įjungimas:</td><td><input type="checkbox" id="Bo_Termostatas" name="Bo_Termostatas"></td></tr>
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="myButton" value="Įrašyti"></td></tr>

</table>
</form>
<div align="center"><a href="/"   style="width:220px"  class="myButton" >Pradžia</a></div>
<br>
<div align="left">
<span class="textas">Boilerio įjungimo t (°C)</span><span class="textas1">- jei temperatūra žemesnė, o katile yra šilumos daugiau, įjungiamas boilerio siurbys.</span><br><br>
<span class="textas">Boilerio išjungimo t (°C)</span></span><span class="textas1">- pasiekus nustatytą temperatūrą boilerio siurbys išjungiamas.<br><br>
<span class="textas">Termostato įjungimas</span></span><span class="textas1">- pažymėjus įjungiamas boilerio šildymas elektra.</span></div>


</div></div>
<script>
  

window.onload = function ()
{  load("style.css","css", function() 
  {    load("microajax.js","js", function() 
    {        setValues("/admin/BoilerisValues");
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


void send_BoilerioKonfiguracija_html()
{
  if (server.args() > 0 )  // Save Settings
  {
    config.Bo_Termostatas = false;
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      if (server.argName(i) == "Bo_ON_T") config.Bo_ON_T = server.arg(i).toInt();
      if (server.argName(i) == "Bo_OFF_T") config.Bo_OFF_T =  server.arg(i).toInt(); 
      if (server.argName(i) == "Bo_Termostatas") config.Bo_Termostatas = true; 

    }
    WriteConfig();
    firstStart = true;
  }
  server.send ( 200, "text/html", PAGE_BoilerioKonfiguracija ); 
  Serial.println(__FUNCTION__); 
  
}

void send_BoilerioKonfiguracija_values_html()
{  
  String values ="";
  values += "Bo_ON_T|" + (String) config.Bo_ON_T + "|input\n";
  values += "Bo_OFF_T|" + (String) config.Bo_OFF_T + "|input\n";
  values += "Bo_Termostatas|" + (String) (config.Bo_Termostatas ? "checked" : "") + "|chk\n";


  server.send ( 200, "text/plain", values);
  Serial.println(__FUNCTION__);
  Serial.print("Bo_ON_T : ");Serial.println(config.Bo_ON_T); 
  Serial.print("Bo_OFF_T : ");Serial.println(config.Bo_OFF_T); 
  Serial.print("Bo_Termostatas : ");Serial.println(config.Bo_Termostatas); 

  
}
