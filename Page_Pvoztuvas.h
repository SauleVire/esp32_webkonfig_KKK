
const char PAGE_PVoztuvoKonfiguracija[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<div class="smartphone">
  <div class="content">
<center><a href="admin.html"  class="myButton"><</a><span class="textas"> PVožtuvo nustatymai </span>
<a href="emoncms.html"  class="myButton">></a>

<hr>
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" >
<tr><td align="right">PV palaikoma t (°C) :</td><td><input type="text" id="pv_palaikoma_t" name="pv_palaikoma_t" min="1" max="2" size="3" maxlength="2" value=""></td></tr>
<tr><td align="right">PV tolerancijos t (°C) :</td><td><input type="text" id="pv_tolerancijos_t" name="pv_tolerancijos_t" min="1" max="9" size="3" maxlength="6" value=""></td></tr>
<tr><td align="right">PV įjungimo t (°C) :</td><td><input type="text" id="pv_on_t" name="pv_on_t" min="1" max="2" size="3" maxlength="3" value=""></td></tr>
<tr><td align="right">PV išjungimo t (°C) :</td><td><input type="text" id="pv_off_t" name="pv_off_t" min="1" max="2" size="3" maxlength="3" value=""></td></tr>
<tr><td align="right">PV rankinis įjungimas t (°C) :</td><td><input type="checkbox" id="pv_rankinis_ijungimas" name="pv_rankinis_ijungimas"></td></tr>
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="myButton" value="Įrašyti"></td></tr>
</table>
</form>
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
        setValues("/admin/pvoztuvovalues");
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


void send_PVoztuvoKonfiguracija_html()
{
  if (server.args() > 0 )  // Save Settings
  {
    config.PV_rankinis_ijungimas = false;
    String temp = "";
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      if (server.argName(i) == "pv_palaikoma_t") config.PV_palaikoma_T = server.arg(i).toFloat();
      if (server.argName(i) == "pv_tolerancijos_t") config.PV_tolerancijos_T = server.arg(i).toFloat();
      if (server.argName(i) == "pv_on_t") config.PV_ON_T =  server.arg(i).toInt(); 
      if (server.argName(i) == "pv_off_t") config.PV_OFF_T =  server.arg(i).toInt(); 
      if (server.argName(i) == "pv_rankinis_ijungimas") config.PV_rankinis_ijungimas = true; 

    }
    WriteConfig();
    firstStart = true;
  }
  server.send ( 200, "text/html", PAGE_PVoztuvoKonfiguracija ); 
  Serial.println(__FUNCTION__); 
  
}

void send_PVoztuvoKonfiguracija_values_html()
{  
  String values ="";
  values += "pv_palaikoma_t|" +  (String) config.PV_palaikoma_T + "|input\n";
  values += "pv_tolerancijos_t|" +  (String) config.PV_tolerancijos_T + "|input\n";
  values += "pv_on_t|" +  (String) config.PV_ON_T + "|input\n";
  values += "pv_off_t|" + (String) config.PV_OFF_T + "|input\n";
  values += "pv_rankinis_ijungimas|" + (String) (config.PV_rankinis_ijungimas ? "checked" : "") + "|chk\n";


  server.send ( 200, "text/plain", values);
  Serial.println(__FUNCTION__);
  Serial.print("pv_palaikoma_t : ");Serial.println(config.PV_palaikoma_T); 
  Serial.print("pv_tolerancijos_t : ");Serial.println(config.PV_tolerancijos_T); 
  Serial.print("pv_on_t : ");Serial.println(config.PV_ON_T); 
  Serial.print("pv_off_t : ");Serial.println(config.PV_OFF_T); 
  Serial.print("pv_rankinis_ijungimas : ");Serial.println(config.PV_rankinis_ijungimas); 
  
}
