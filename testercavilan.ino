#include <SPI.h>
#include <Ethernet.h>
#include <Arduino.h>
#include <U8x8lib.h>   //https://github.com/olikraus/u8g2
#include <ArduinoJson.h>  //https://arduinojson.org/
#include <Dns.h>

byte mac[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x00};                 //MAC Address della scheda di rete sul DHCP
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);
char server_url[] = "time.inrim.it";
byte server_ip[] = { 193, 204, 114, 105 };            //IP del sito time.inrim.it

void setup() {
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);            //pin reset W5100
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.clear();
  u8x8.setFlipMode(1);             //ruota display di 180 gradi
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
  u8x8.drawString(0, 0, "  WAIT FOR DHCP");
}

void loop() {
  if (!Ethernet.begin(mac)) {                //cerca un DHCP che gli dia un'IP per massimo 1 minuto
    u8x8.clear();
    u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
    u8x8.drawString(0, 0, "  NO DHCP");
    blink_led();                            //accende e spegne il LED rete sullo switch per 6 secondi
  }

  u8x8.clear();
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);    //visualizza per 10 secondi parametri di base
  u8x8.drawString(0, 0, "     IP:");
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0, 1, ip2CharArray(Ethernet.localIP()));
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
  u8x8.drawString(0, 2, "     SUNBET:");
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0, 3, ip2CharArray(Ethernet.subnetMask()));
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
  u8x8.drawString(0, 4, "     GATEWAY:");
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0, 5, ip2CharArray(Ethernet.gatewayIP()));
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
  u8x8.drawString(0, 6, "     DNS:");
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0, 7, ip2CharArray(Ethernet.dnsServerIP()));
  delay(10000);
  u8x8.clear();

  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
  u8x8.drawString(0, 0, " WAIT FOR WEB");
  EthernetClient client;
  client.setTimeout(10000);
  if (!client.connect("34.117.186.192", 80)) {      // cerca di andare su internet per ottenere i dati della linea internet, senza usare i DNS...
    u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
    u8x8.drawString(0, 0, "  NO INTERNET");
    blink_led();                                   //se non ci riesce, accende e spegne il LED rete sullo switch per 6 secondi
    return;
  }
  client.println(F("GET /json HTTP/1.0"));
  client.println(F("Host: ipinfo.io"));
  client.println(F("Connection: close"));
  if (client.println() == 0) {
    u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
    u8x8.drawString(0, 0, "  NO INTERNET");
    client.stop();
    blink_led();                                  //se non ci riesce, accende e spegne il LED rete sullo switch per 6 secondi
    return;
  }
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
  if (strcmp(status + 9, "200 OK") != 0) {
    u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
    u8x8.drawString(0, 0, "  NO INTERNET");
    client.stop();
    blink_led();                               //se non ci riesce, accende e spegne il LED rete sullo switch per 6 secondi
    return;
  }
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
    u8x8.drawString(0, 0, "  NO INTERNET");
    client.stop();
    blink_led();                            //se non ci riesce, accende e spegne il LED rete sullo switch per 6 secondi
    return;
  }
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, client);
  if (error) {
    u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
    u8x8.drawString(0, 0, "  NO INTERNET");
    client.stop();
    blink_led();                          //se non ci riesce, accende e spegne il LED rete sullo switch per 6 secondi
    return;
  }
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);   //ha contattato con successo http://ipinfo.io/json per avere i dati della linea (IP, Operatore, Posizione)
  u8x8.drawString(0, 0, "     WAN IP:  ");
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  const char* wanip = doc["ip"].as<const char*>();
  u8x8.drawString(0, 1, wanip);
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
  u8x8.drawString(0, 2, "     ISP:");
  const char* operatore = doc["org"].as<const char*>();
  char* dummy = "";
  operatore = strstr( operatore, " ");
  operatore = strncat(dummy, operatore, 16);
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0, 3, operatore);
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
  u8x8.drawString(0, 4, "    LOCATION:");
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  const char* city = doc["city"].as<const char*>();
  u8x8.drawString(0, 5, city);
  const char* region = doc["region"].as<const char*>();
  u8x8.drawString(0, 6, region);
  delay(10000);
  client.stop();

  u8x8.clear();
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
  u8x8.drawString(0, 0, "    TEST DNS:");
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0, 1, "time.inrim.it =");
  u8x8.drawString(0, 2, "WAIT");
  DNSClient dns_client;
  dns_client.begin(Ethernet.dnsServerIP());            // prova a risolvere tramite il DNS il sito time.inrim.it
  IPAddress server_ip_resolved;
  if (dns_client.getHostByName(server_url, server_ip_resolved) == 1 && server_ip_resolved == server_ip) {
    u8x8.drawString(0, 2, ip2CharArray(server_ip_resolved));
    u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
    u8x8.drawString(0, 3, "    OK");                  //se restituisce 193.204.114.105 allora è OK
    blink_led();                                      //accende e spegne il LED rete sullo switch per 6 secondi
  } else {
    u8x8.drawString(0, 2, ip2CharArray(server_ip_resolved));
    u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
    u8x8.drawString(0, 3, "    FAIL");               //altrimenti va in FAIL mostrando un risultato farlocco
    blink_led();                                     //accende e spegne il LED rete sullo switch per 6 secondi
  }
}

char* ip2CharArray(IPAddress ip) {
  static char a[16];
  sprintf(a, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  return a;
}

void blink_led() {
  u8x8.setFont(u8x8_font_amstrad_cpc_extended_r);
  u8x8.drawString(0, 4, "BLINKING (6sec)");
  while (true) {
    digitalWrite(4, LOW);
    u8x8.drawString(0, 6, "    OFF");
    delay(5000);
    digitalWrite(4, HIGH);
    delay(1000);
    u8x8.drawString(0, 6, "     ON");
    delay(6000);
  }
}
