
const char PAGE_KolektoriausKonfiguracija[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<div class="smartphone">
  <div class="content">
<center><a href="admin.html"  class="myButton"><</a><span class="textas"> Kolektoriaus nustatymai </span>
<a href="boileris.html"  class="myButton">></a>

<hr>
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" >
<tr><td align="right">Skirtumas (°C) :</td><td><input type="text" id="k_skirtumas" name="k_skirtumas" min="1" max="30" size="3" maxlength="4" value=""></td></tr>
<tr><td align="right">Laiko intervalas (s) :</td><td><input type="text" id="k_intervalas" name="k_intervalas" min="1" max="600" size="3" maxlength="3" value=""></td></tr>
<tr><td align="right">Apsauga nuo užšalimo :</td><td><input type="checkbox" id="k_uzsalimas" name="k_uzsalimas"></td></tr>
<tr><td align="right">Nuorinimas :</td><td><input type="checkbox" id="k_nuorinimas" name="k_nuorinimas"></td></tr>
<tr><td align="right">Kp :</td><td>        <input type="text" id="Kp"         name="Kp"         min="1" max="60000" size="6" value="""></td></tr>
<tr><td align="right">Ki :</td><td>        <input type="text" id="Ki"         name="Ki"         min="1" max="60000" size="6"value=""></td></tr>
<tr><td align="right">Kd :</td><td>        <input type="text" id="Kd"         name="Kd"         min="1" max="60000" size="6"value=""></td></tr>
<tr><td align="right">WindowSize :</td><td><input type="text" id="WindowSize" name="WindowSize" min="1" max="60000" size="6" value=""></td></tr>
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="myButton" value="Įrašyti"></td></tr>
</table>
</form>
<div align="center"><a href="/"   style="width:220px"  class="myButton" >Pradžia</a></div>
<br><br>
<a href="https://saulevire.lt/emoncms9/dashboard/view/testas?apikey=read">sukaupti duomenys</a>

</div></div>
<script>
  

window.onload = function ()
{
  load("style.css","css", function() 
  {
    load("microajax.js","js", function() 
    {
        setValues("/admin/kolektoriusvalues");
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


void send_KolektoriausKonfiguracija_html()
{
  if (server.args() > 0 )  // Save Settings
  {
    config.k_uzsalimas = false;
    config.k_nuorinimas = false;
    String temp = "";
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      if (server.argName(i) == "k_skirtumas") config.k_skirtumas = server.arg(i).toFloat();
      if (server.argName(i) == "k_intervalas") config.k_intervalas =  server.arg(i).toInt(); 
      if (server.argName(i) == "Kp") config.Kp = server.arg(i).toFloat();
      if (server.argName(i) == "Ki") config.Ki = server.arg(i).toFloat();
      if (server.argName(i) == "Kd") config.Kd = server.arg(i).toFloat();
      if (server.argName(i) == "WindowSize") config.WindowSize = server.arg(i).toInt();
      if (server.argName(i) == "k_uzsalimas") config.k_uzsalimas = true; 
      if (server.argName(i) == "k_nuorinimas") config.k_nuorinimas = true; 

    }
    WriteConfig();
    firstStart = true;
  }
  server.send ( 200, "text/html", PAGE_KolektoriausKonfiguracija ); 
  Serial.println(__FUNCTION__); 
  
}

void send_KolektoriausKonfiguracija_values_html()
{  
  String values ="";
  values += "k_skirtumas|" +  (String) config.k_skirtumas + "|input\n";
  values += "k_intervalas|" +  (String) config.k_intervalas + "|input\n";
  values += "Kp|" + (String) config.Kp + "|input\n";
  values += "Ki|" + (String) config.Ki + "|input\n";
  values += "Kd|" + (String) config.Kd + "|input\n";
  values += "WindowSize|" + (String) config.WindowSize + "|input\n";
  values += "k_uzsalimas|" + (String) (config.k_uzsalimas ? "checked" : "") + "|chk\n";
  values += "k_nuorinimas|" + (String) (config.k_nuorinimas ? "checked" : "") + "|chk\n";


  server.send ( 200, "text/plain", values);
  Serial.println(__FUNCTION__);
  Serial.print("k_uzsalimas : ");Serial.println(config.k_uzsalimas); 
  Serial.print("k_nuorinimas : ");Serial.println(config.k_nuorinimas); 
  Serial.print("k_intervalas : ");Serial.println(config.k_intervalas); 
  Serial.print("k_skirtumas : ");Serial.println(config.k_skirtumas); 
  Serial.print("Kp : ");Serial.println(config.Kp); 
  Serial.print("Ki : ");Serial.println(config.Ki); 
  Serial.print("Kd : ");Serial.println(config.Kd); 
  Serial.print("WindowSize : ");Serial.println(config.WindowSize); 
  
}
