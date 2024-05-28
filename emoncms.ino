/*
//#define emoncms_host "1.2.3.4"
const int emoncms_httpPort = 80;
String emoncms_apikey = "YOUR_API_KEY";
int node = 1;

//config.emoncmsSrv; 
//(char*) config.emoncmsSrv.c_str();


void postEmoncms() {
  // run only if the ESP is connected to a network
  if ( WiFi.status() == WL_CONNECTED ) {
    WiFiClient client;
    if (!client.connect(config.emoncmsSrv.c_str(), emoncms_httpPort)) {
      Serial.println("Post to emoncms failed");
      return;
    }
    // We now create a URI for the request
    String url = "GET /emoncms/input/post.json?apikey=" + emoncms_apikey + "&node=" + node + "&json={2:" + Kolektorius + ",1:" + OrasL + "";
    client.println(url);
    client.print("Host: ");
    client.println(config.emoncmsSrv);
    client.println("Connection: close");
    client.println();
    client.stop(); //Stopping client
    Serial.println("Post to emoncms : success");
  }
  else {
    Serial.println("Post to emoncms : not connected to internet");
  }
}
*/

void emoncms(){

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(config.emoncmsSrv.c_str(), httpPort)) {
    Serial.println("connection failed");
    return;
  }

//String url = "/emoncms/input/post.json?node=" + String(config.katalogas) + "&json={"+ String(config.reiksme1)+":"+String(Kolektorius)+"," + String(config.reiksme2)+ ":"+String(Boileris)+","+ String(config.reiksme3)+":"+String(OrasL)+"}&apikey="+String(config.apikey);
String url = "/input/post.json?node=";
       url += String(config.katalogas)+ "&json={";
       url += String(config.reiksme1)+":"+String(Kolektorius)+",";
       url += String(config.reiksme2)+ ":"+String(Boileris)+",";
       url += String(config.reiksme3)+":"+String(OrasL);
       url += "}&apikey="+String(config.apikey);
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + String(config.emoncmsSrv) + "\r\n" + 
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 750) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  
}
