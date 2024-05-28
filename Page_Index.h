#ifndef PAGE_EXAMPLE_H
#define PAGE_EXAMPLE_H
//
//   The EXAMPLE PAGE
//
const char PAGE_EXAMPLE[] PROGMEM = R"=====(
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<div class="smartphone">
  <div class="content">

<table border="0"  cellspacing="0" cellpadding="3" style="width:300px" >

<tr>
<th colspan="1"><a href="/admin.html" class="myButton">Valdiklio konfigūravimas</a></th>
<th><div class='wifi'></div></th>
<th><div><span id="wifiSignal"></span></div></th>
</tr>

<tr>
<th colspan="3"><b>Dabartinė KKK ūkio būsena</b></th>
</tr>

<tr>
<th colspan="1">
<div align="right">Kolektorius :</div>
<div align="right">Oras :</div>
<div align="right">Kolektoriaus siurblio būsena :</div>
<div align="right">Laiko intervelas :</div>
<div align="right">Užšalimo apsauga :</div>
<div align="right">Nuorinimas :</div>
<div align="right">Žiema :</div>
<br>
<div align="right">Boileris t :</div>
<div align="right">Boilerio įjungimo t :</div>
<div align="right">Boilerio išjungimo t :</div>
<div align="right">Boilerio termostatas :</div>
<br>
<div align="right">Akumuliacinės viršus t :</div>
<div align="right">Akumuliacinės apačia t :</div>
<div align="right">Ak. talpos įjungimo t :</div>
<div align="right">Ak. talpos išjungimo t :</div>
<div align="right">Ak. talpos siurblys :</div>
<div align="right">Ak. talpos rankinis valdymas :</div>
<br>
<div align="right">PVožtuvo palaikoma t :</div>
<div align="right">PVožtuvo tolerancijos t :</div>
<div align="right">PVožtuvo įjungimo t :</div>
<div align="right">PVožtuvo išjungimo t :</div>
<div align="right">PVožtuvo rankinis valdymas :</div>

</th>
<th colspan="2">
<div><span id="K_t"></span>&deg; C</div>
<div><span id="O_t"></span>&deg; C</div> 
<div><span id="rele"></span>&nbsp;</div> 
<div><span id="laikas"></span> (s)</div> 
<div><span id="apsauga"></span></div> 
<div><span id="nuorinimas"></span></div>
<div><span id="ziema"></span></div>
<br>
<div><span id="B_t"></span>&deg; C</div>
<div><span id="B_on_t"></span>&deg; C</div>
<div><span id="B_off_t"></span>&deg; C</div>
<div><span id="Bo_termostatas"></span>&nbsp;</div>
<br>
<div><span id="AV_t"></span>&deg; C</div>
<div><span id="AA_t"></span>&deg; C</div>
<div><span id="A_on_t"></span>&deg; C</div>
<div><span id="A_off_t"></span>&deg; C</div>
<div><span id="AK_siurblys"></span>&deg; C</div>
<div><span id="AK_rankinis"></span>&nbsp;</div>
<br>
<div><span id="PV_palaikoma"></span>&deg; C</div>
<div><span id="PV_tolerancijos"></span>&deg; C</div>
<div><span id="PV_on"></span>&deg; C</div>
<div><span id="PV_off"></span>&deg; C</div>
<div><span id="PV_rankinis"></span>&nbsp;</div>

</th></tr>
<tr><th colspan=3><a href="/"  class="myButton">Atnaujinti</a></th></tr>
</table>
<hr>
<span class="textas">Dabar : <span id="x_ntp"></span><br>
<!-- Valdiklis veikia <span id="x_ntp2"></span> nuo <span id="x_ntp2"></span><br></span> -->



<p><small><a href=https://saulevire.lt>SauleVire.lt</a> © 2019</small></p>
</div>
</div></center>
<!-- <div id="K_t">Here comes the Dynamic Data in </div> -->
<!-- added a DIV, where the dynamic data goes to -->
   <script>                
		window.onload = function ()
		{	load("style.css","css", function() 
			{	load("microajax.js","js", function() 
				{	setValues("/admin/filldynamicdata");  //-- this function calls the function on the ESP      
				});
			});
		}
		function load(e,t,n){
		  if("js"==t){var a=document.createElement("script");
		a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}
		else if("css"==t){var a=document.createElement("link");
		a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}
   </script>

)=====";
#endif

String WiFiSignal() {
  float Signal = WiFi.RSSI();
  Signal = 90 / 40.0 * Signal + 212.5; // From Signal = 100% @ -50dBm and Signal = 10% @ -90dBm and y = mx + c
  if (Signal > 100) Signal = 100;
  return " " + String(Signal, 0) + "%";
}

void filldynamicdata()
{        
    String values ="";
  values += "wifiSignal|" + (String)(WiFiSignal())+ "|div\n";
  values += "K_t|" + (String)Kolektorius +  "|div\n";
  values += "O_t|" + (String)OrasL +  "|div\n";
  values += "rele|" + (String)CollectorState +  "|div\n";
  values += "laikas|" + (String) config.k_intervalas  +  "|div\n";
  values += "apsauga|" + (String)FreezingState +  "|div\n";
  values += "nuorinimas|" + (String)Nuorinimas +  "|div\n";
  values += "ziema|" + (String)Ziema +  "|div\n";
  
  values += "B_t|" + (String)Boileris +  "|div\n";
  values += "B_on_t|" + (String)config.Bo_ON_T +  "|div\n";
  values += "B_off_t|" + (String)config.Bo_OFF_T +  "|div\n";
  values += "Bo_termostatas|" + (String)Bo_thermostatState +  "|div\n";

  values += "AV_t|" + (String)AkumuliacineV +  "|div\n";
  values += "AA_t|" + (String)AkumuliacineA +  "|div\n";
  values += "A_on_t|" + (String)config.At_ON_T +  "|div\n";
  values += "A_off_t|" + (String)config.At_OFF_T +  "|div\n";
  values += "AK_siurblys|" + (String)config.At_Siurblio_busena +  "|div\n";
  values += "AK_rankinis|" + (String)config.At_Rankinis_ijungimas +  "|div\n";

  values += "PV_palaikoma|" + (String)config.PV_palaikoma_T +  "|div\n";
  values += "PV_tolerancijos|" + (String)config.PV_tolerancijos_T +  "|div\n";
  values += "PV_on|" + (String)config.PV_ON_T +  "|div\n";
  values += "PV_off|" + (String)config.PV_OFF_T +  "|div\n";
  values += "PV_rankinis|" + (String)config.PV_rankinis_ijungimas +  "|div\n";

  values += "x_ntp|" + (String)DateTime.year + "." + (String)DateTime.month + "." + (String)DateTime.day + " " + (String)DateTime.hour + ":" + (String)DateTime.minute + ":" + (String)DateTime.second + " |div\n";
//  values += "x_ntp2|" + (String)(NTP.getTimeDateString (NTP.getFirstSync ()).c_str ())+ " |div\n";
//  values += "x_ntp1|" + (String)(NTP.getUptimeString ())+ " |div\n";
    server.send ( 200, "text/plain", values); 
}

void processIndex()
{        
    if (server.args() > 0 )  // Are there any POST/GET Fields ? 
    {
       for ( uint8_t i = 0; i < server.args(); i++ ) {  // Iterate through the fields
            if (server.argName(i) == "firstname") 
            {
                 // Your processing for the transmitted form-variable 
                 String fName = server.arg(i);
            }
        }
    }
    server.send ( 200, "text/html", PAGE_EXAMPLE  ); 
}
