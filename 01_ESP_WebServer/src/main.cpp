#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// Replace these values with your Wi-Fi details.
const char *WIFI_SSID = "YOUR_WIFI_NAME";
const char *WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

WebServer server(80);

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

bool ledOn = false;

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta name="theme-color" content="#061426">
  <title>IoTecotech | Smart IoT</title>
  <style>
    :root{--bg:#061426;--panel:#0a1d33;--line:#193b58;--accent:#12d4df;--text:#f4f8fc;--muted:#9db0c3;--green:#42e58b}
    *{box-sizing:border-box}
    html{scroll-behavior:smooth}
    body{margin:0;background:var(--bg);color:var(--text);font-family:Arial,Helvetica,sans-serif;line-height:1.5}
    a{color:inherit;text-decoration:none}
    .wrap{width:min(1120px,calc(100% - 40px));margin:auto}
    header{border-bottom:1px solid var(--line)}
    nav{height:74px;display:flex;align-items:center;justify-content:space-between}
    .logo{font-size:25px;font-weight:800;letter-spacing:-1px}.logo span{color:var(--accent)}
    .nav-links{display:flex;gap:30px;color:#c9d5e0;font-size:14px}.nav-links a:hover{color:var(--accent)}
    .hero{min-height:540px;display:grid;grid-template-columns:.85fr 1.15fr;align-items:center;gap:65px;padding:68px 0}
    h1{font-size:clamp(42px,5.5vw,72px);line-height:1.04;letter-spacing:-3px;margin:0 0 24px;max-width:570px}
    h1 span{color:var(--accent)}
    .intro{color:var(--muted);font-size:18px;max-width:500px;margin:0 0 32px}
    .cta{display:inline-block;background:var(--accent);color:#00131b;padding:14px 24px;border-radius:8px;font-weight:800;transition:.2s}
    .cta:hover{transform:translateY(-2px);box-shadow:0 10px 30px #12d4df35}
    .status-panel{background:var(--panel);border:1px solid var(--accent);border-radius:16px;padding:26px;box-shadow:0 24px 70px #0006}
    .panel-head{display:flex;align-items:center;justify-content:space-between;padding-bottom:18px;border-bottom:1px solid var(--line)}
    .panel-title{font-size:20px;font-weight:700}.online{display:flex;align-items:center;gap:8px;font-size:14px;color:#cbd8e2}
    .dot{width:9px;height:9px;border-radius:50%;background:var(--green);box-shadow:0 0 10px #42e58b99}
    .device{display:grid;grid-template-columns:1fr auto;align-items:center;gap:16px;padding:19px 0;border-bottom:1px solid var(--line)}
    .device:last-child{border-bottom:0;padding-bottom:0}.device-main{display:flex;align-items:center;gap:14px}
    .icon{width:38px;height:38px;display:grid;place-items:center;border:1px solid #257191;border-radius:10px;color:var(--accent);font-size:19px}
    .device small{display:block;color:var(--muted);margin-top:2px}.device-state{color:var(--green);font-size:14px}
    .switch{position:relative;width:48px;height:26px;border:0;border-radius:99px;background:#27455c;cursor:pointer;transition:.2s}
    .switch:after{content:"";position:absolute;width:20px;height:20px;left:3px;top:3px;background:white;border-radius:50%;transition:.2s}
    .switch.on{background:var(--accent)}.switch.on:after{transform:translateX(22px)}
    .features{border-top:1px solid var(--line);padding:54px 0 60px}
    .feature-grid{display:grid;grid-template-columns:repeat(3,1fr);gap:0}
    .feature{padding:4px 42px;border-right:1px solid var(--line)}.feature:first-child{padding-left:0}.feature:last-child{border:0;padding-right:0}
    .feature h2{font-size:18px;color:var(--accent);margin:0 0 10px}.feature p{font-size:14px;color:var(--muted);margin:0}
    footer{border-top:1px solid var(--line);text-align:center;color:#71889c;padding:24px;font-size:13px}footer span{color:var(--accent)}
    @media(max-width:800px){.hero{grid-template-columns:1fr;gap:42px;padding:55px 0}.hero-copy{text-align:center}.intro{margin-left:auto;margin-right:auto}h1{letter-spacing:-2px;margin-left:auto;margin-right:auto}.feature-grid{grid-template-columns:1fr}.feature,.feature:first-child,.feature:last-child{padding:24px 0;border-right:0;border-bottom:1px solid var(--line)}.feature:last-child{border-bottom:0}.nav-links{gap:16px}}
    @media(max-width:480px){.wrap{width:min(100% - 26px,1120px)}nav{height:64px}.nav-links a:not(:first-child){display:none}.status-panel{padding:19px}.device{font-size:14px}h1{font-size:43px}.hero{padding-top:42px}}
    @media(prefers-reduced-motion:reduce){html{scroll-behavior:auto}.cta,.switch,.switch:after{transition:none}}
  </style>
</head>
<body>
  <header id="home"><nav class="wrap"><a class="logo" href="#home">Io<span>Teco</span>tech</a><div class="nav-links"><a href="#home">Home</a><a href="#features">Features</a><a href="#about">About</a></div></nav></header>
  <main>
    <section class="hero wrap">
      <div class="hero-copy">
        <h1>Smart living starts with <span>IoTecotech</span></h1>
        <p class="intro">Connect, monitor, and control your devices anytime, anywhere with a simple IoT experience.</p>
        <a class="cta" href="#features">Explore Features</a>
      </div>
      <div class="status-panel" aria-label="Device status">
        <div class="panel-head"><div class="panel-title">Device Status</div><div class="online"><i class="dot"></i>ESP32 Online</div></div>
        <div class="device"><div class="device-main"><div class="icon">&#9728;</div><div>Board LED<small>GPIO 2</small></div></div><div><span class="device-state" id="ledState">Off</span> <button class="switch" id="ledSwitch" aria-label="Toggle board LED" aria-pressed="false"></button></div></div>
        <div class="device"><div class="device-main"><div class="icon">&#8451;</div><div>Room Temperature<small>Live sensor preview</small></div></div><strong>24&deg;C</strong></div>
        <div class="device"><div class="device-main"><div class="icon">&#9889;</div><div>Smart Plug<small>Living room</small></div></div><span class="device-state">On</span></div>
      </div>
    </section>
    <section class="features" id="features"><div class="wrap feature-grid">
      <article class="feature"><h2>Smart Control</h2><p>Control connected devices remotely with straightforward tools and automation.</p></article>
      <article class="feature"><h2>Live Monitoring</h2><p>See device status and activity in real time from one clear interface.</p></article>
      <article class="feature" id="about"><h2>Secure Connection</h2><p>Keep your data and devices protected on your trusted local network.</p></article>
    </div></section>
  </main>
  <footer>&copy; 2026 <span>IoTecotech</span>. Built for a smarter connected world.</footer>
  <script>
    const button=document.getElementById('ledSwitch');
    const state=document.getElementById('ledState');
    async function setLed(on){
      button.classList.toggle('on',on);button.setAttribute('aria-pressed',on);state.textContent=on?'On':'Off';
      try{const response=await fetch('/api/led?state='+(on?1:0));if(!response.ok)throw new Error();}
      catch(error){button.classList.toggle('on',!on);button.setAttribute('aria-pressed',!on);state.textContent=!on?'On':'Off';}
    }
    button.addEventListener('click',()=>setLed(!button.classList.contains('on')));
    fetch('/api/status').then(r=>r.json()).then(data=>{button.classList.toggle('on',data.led);button.setAttribute('aria-pressed',data.led);state.textContent=data.led?'On':'Off';}).catch(()=>{});
  </script>
</body>
</html>
)rawliteral";

void handleLed() {
  if (!server.hasArg("state")) {
    server.send(400, "application/json", "{\"error\":\"Missing state\"}");
    return;
  }

  ledOn = server.arg("state") == "1";
  // Most ESP32 development boards use an active-high built-in LED.
  digitalWrite(LED_BUILTIN, ledOn ? HIGH : LOW);
  server.send(200, "application/json", ledOn ? "{\"led\":true}" : "{\"led\":false}");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
  }

  Serial.println("\nConnected!");
  Serial.print("Open this address in your browser: http://");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html", INDEX_HTML);
  });

  server.on("/api/status", HTTP_GET, []() {
    server.send(200, "application/json", ledOn ? "{\"led\":true}" : "{\"led\":false}");
  });

  server.on("/api/led", HTTP_GET, handleLed);
  server.onNotFound([]() {
    server.send(404, "text/plain", "Not found");
  });

  server.begin();
  Serial.println("IoTecotech web server started.");
}

void loop() {
  server.handleClient();
}
