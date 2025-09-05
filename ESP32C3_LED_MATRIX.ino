#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <SPI.h>

// MAX7219 LED Matrix config
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define DIN_PIN 0
#define CLK_PIN 1
#define CS_PIN 2

MD_Parola P = MD_Parola(HARDWARE_TYPE, DIN_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Captive Portal config
const char* ssid = "ESP32C3 LED_MATRIX"; // AP name
AsyncWebServer server(80);

String displayTextStr = "WELCOME!"; // Default text
int scrollSpeed = 50; // Default scroll speed (ms)

void setup() {
  Serial.begin(115200);

  // Initialize LED matrix
  P.begin();
  P.setIntensity(5);
  P.displayText(displayTextStr.c_str(), PA_CENTER, scrollSpeed, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);

  // Setup AP mode
  WiFi.softAP(ssid);
  Serial.println("AP started. Connect to ESP32_C3_AP");

  // Serve HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = R"rawliteral(
    <!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 LED Matrix Control</title>
  <style>
    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
    }

    body {
      font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, sans-serif;
      background: #0a0a0a;
      color: #ffffff;
      min-height: 100vh;
      display: flex;
      flex-direction: column;
      position: relative;
      overflow-x: hidden;
    }

    /* Animated background */
    body::before {
      content: '';
      position: fixed;
      top: 0;
      left: 0;
      width: 100%;
      height: 100%;
      background: radial-gradient(circle at 25% 25%, #1a1a2e 0%, transparent 50%),
                  radial-gradient(circle at 75% 75%, #16213e 0%, transparent 50%),
                  linear-gradient(135deg, #0f0f0f 0%, #1a1a1a 100%);
      z-index: -2;
    }

    /* Subtle grid pattern */
    body::after {
      content: '';
      position: fixed;
      top: 0;
      left: 0;
      width: 100%;
      height: 100%;
      background-image: 
        linear-gradient(rgba(255,255,255,0.02) 1px, transparent 1px),
        linear-gradient(90deg, rgba(255,255,255,0.02) 1px, transparent 1px);
      background-size: 20px 20px;
      z-index: -1;
    }

    main {
      flex: 1;
      display: flex;
      align-items: center;
      justify-content: center;
      padding: 2rem 1rem;
    }

    .container {
      background: rgba(255, 255, 255, 0.03);
      backdrop-filter: blur(20px);
      border: 1px solid rgba(255, 255, 255, 0.1);
      padding: 3rem 2rem;
      border-radius: 24px;
      width: 100%;
      max-width: 480px;
      box-shadow: 
        0 25px 50px -12px rgba(0, 0, 0, 0.25),
        0 0 0 1px rgba(255, 255, 255, 0.05),
        inset 0 1px 0 rgba(255, 255, 255, 0.1);
      position: relative;
    }

    .container::before {
      content: '';
      position: absolute;
      top: 0;
      left: 0;
      right: 0;
      height: 1px;
      background: linear-gradient(90deg, transparent, rgba(255, 255, 255, 0.2), transparent);
    }

    h1 {
      font-size: 1.875rem;
      font-weight: 700;
      margin-bottom: 2rem;
      background: linear-gradient(135deg, #ffffff, #a0a0a0);
      -webkit-background-clip: text;
      -webkit-text-fill-color: transparent;
      background-clip: text;
      text-align: center;
      letter-spacing: -0.025em;
    }

    .form-group {
      margin-bottom: 1.5rem;
      position: relative;
    }

    .form-group:last-child {
      margin-bottom: 0;
    }

    label {
      display: block;
      margin-bottom: 0.5rem;
      font-size: 0.875rem;
      font-weight: 500;
      color: #a0a0a0;
      text-transform: uppercase;
      letter-spacing: 0.05em;
    }

    input[type="text"], 
    input[type="number"] {
      width: 100%;
      padding: 1rem 1.25rem;
      background: rgba(255, 255, 255, 0.05);
      border: 1px solid rgba(255, 255, 255, 0.1);
      border-radius: 12px;
      color: #ffffff;
      font-size: 1rem;
      font-weight: 400;
      transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
      outline: none;
    }

    input[type="text"]:focus, 
    input[type="number"]:focus {
      border-color: rgba(99, 102, 241, 0.5);
      box-shadow: 0 0 0 3px rgba(99, 102, 241, 0.1);
      background: rgba(255, 255, 255, 0.08);
    }

    input[type="text"]::placeholder, 
    input[type="number"]::placeholder {
      color: #6b7280;
    }

    .submit-btn {
      width: 100%;
      padding: 1rem 1.5rem;
      background: linear-gradient(135deg, #6366f1, #8b5cf6);
      border: none;
      border-radius: 12px;
      color: #ffffff;
      font-size: 1rem;
      font-weight: 600;
      cursor: pointer;
      transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
      position: relative;
      overflow: hidden;
      text-transform: uppercase;
      letter-spacing: 0.025em;
    }

    .submit-btn::before {
      content: '';
      position: absolute;
      top: 0;
      left: -100%;
      width: 100%;
      height: 100%;
      background: linear-gradient(90deg, transparent, rgba(255, 255, 255, 0.2), transparent);
      transition: left 0.5s;
    }

    .submit-btn:hover {
      transform: translateY(-2px);
      box-shadow: 0 20px 25px -5px rgba(99, 102, 241, 0.3);
      background: linear-gradient(135deg, #5856ec, #7c3aed);
    }

    .submit-btn:hover::before {
      left: 100%;
    }

    .submit-btn:active {
      transform: translateY(0);
    }

    .submit-btn:disabled {
      opacity: 0.5;
      cursor: not-allowed;
      transform: none;
    }

    /* Status indicator */
    .status {
      margin-top: 1.5rem;
      padding: 1rem;
      border-radius: 8px;
      font-size: 0.875rem;
      font-weight: 500;
      text-align: center;
      opacity: 0;
      transform: translateY(10px);
      transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
    }

    .status.show {
      opacity: 1;
      transform: translateY(0);
    }

    .status.success {
      background: rgba(34, 197, 94, 0.1);
      color: #22c55e;
      border: 1px solid rgba(34, 197, 94, 0.2);
    }

    .status.error {
      background: rgba(239, 68, 68, 0.1);
      color: #ef4444;
      border: 1px solid rgba(239, 68, 68, 0.2);
    }

    /* Footer */
    footer {
      padding: 2rem 1rem;
      text-align: center;
      border-top: 1px solid rgba(255, 255, 255, 0.1);
      background: rgba(0, 0, 0, 0.3);
      backdrop-filter: blur(10px);
    }

    .footer-content {
      max-width: 1200px;
      margin: 0 auto;
    }

    .footer-text {
      font-size: 0.875rem;
      color: #6b7280;
      margin-bottom: 0.5rem;
    }

    .footer-credit {
      font-size: 0.875rem;
      color: #a0a0a0;
      font-weight: 500;
    }

    .footer-credit .highlight {
      background: linear-gradient(135deg, #6366f1, #8b5cf6);
      -webkit-background-clip: text;
      -webkit-text-fill-color: transparent;
      background-clip: text;
      font-weight: 600;
    }

    /* Mobile responsiveness */
    @media (max-width: 640px) {
      .container {
        padding: 2rem 1.5rem;
        border-radius: 16px;
        margin: 1rem;
      }

      h1 {
        font-size: 1.5rem;
        margin-bottom: 1.5rem;
      }

      input[type="text"], 
      input[type="number"] {
        padding: 0.875rem 1rem;
        font-size: 0.95rem;
      }

      .submit-btn {
        padding: 0.875rem 1.25rem;
        font-size: 0.95rem;
      }

      footer {
        padding: 1.5rem 1rem;
      }

      .footer-text,
      .footer-credit {
        font-size: 0.8rem;
      }
    }

    @media (max-width: 480px) {
      main {
        padding: 1rem 0.5rem;
      }

      .container {
        padding: 1.5rem 1rem;
        margin: 0.5rem;
      }

      h1 {
        font-size: 1.25rem;
      }
    }

    /* Animations */
    @keyframes fadeInUp {
      from {
        opacity: 0;
        transform: translateY(30px);
      }
      to {
        opacity: 1;
        transform: translateY(0);
      }
    }

    .container {
      animation: fadeInUp 0.8s cubic-bezier(0.4, 0, 0.2, 1);
    }
  </style>
</head>
<body>
  <main>
    <div class="container">
      <h1>ESP32 LED Matrix Control</h1>
      <form id="matrixForm">
        <div class="form-group">
          <label for="text">Display Text</label>
          <input type="text" id="text" placeholder="Enter your message" required>
        </div>
        
        <div class="form-group">
          <label for="speed">Scroll Speed (ms)</label>
          <input type="number" id="speed" placeholder="50" min="10" max="500" value="50" required>
        </div>
        
        <div class="form-group">
          <button type="submit" class="submit-btn" id="submitBtn">
            Display Message
          </button>
        </div>
        
        <div class="status" id="status"></div>
      </form>
    </div>
  </main>

  <footer>
    <div class="footer-content">
      <div class="footer-text">ESP32 LED Matrix Control Interface</div>
      <div class="footer-credit">Created by <span class="highlight">Roy Cuadra</span></div>
    </div>
  </footer>

  <script>
    const form = document.getElementById('matrixForm');
    const submitBtn = document.getElementById('submitBtn');
    const status = document.getElementById('status');

    function showStatus(message, type = 'success') {
      status.textContent = message;
      status.className = `status ${type} show`;
      
      setTimeout(() => {
        status.classList.remove('show');
      }, 3000);
    }

    form.addEventListener('submit', async (e) => {
      e.preventDefault();
      
      const text = document.getElementById('text').value.trim();
      const speed = document.getElementById('speed').value;
      
      if (!text) {
        showStatus('Please enter some text to display', 'error');
        return;
      }

      // Disable button during request
      submitBtn.disabled = true;
      submitBtn.textContent = 'Updating...';

      try {
        const response = await fetch('/setText?text=' + encodeURIComponent(text) + '&speed=' + encodeURIComponent(speed));
        
        if (response.ok) {
          showStatus('Text and scroll speed updated successfully!', 'success');
        } else {
          throw new Error('Network response was not ok');
        }
      } catch (error) {
        console.error('Error:', error);
        showStatus('Failed to update display. Please try again.', 'error');
      } finally {
        // Re-enable button
        submitBtn.disabled = false;
        submitBtn.textContent = 'Display Message';
      }
    });

    // Add some interactive feedback for inputs
    const inputs = document.querySelectorAll('input');
    inputs.forEach(input => {
      input.addEventListener('focus', (e) => {
        e.target.parentElement.style.transform = 'scale(1.02)';
      });
      
      input.addEventListener('blur', (e) => {
        e.target.parentElement.style.transform = 'scale(1)';
      });
    });
  </script>
</body>
</html>
    )rawliteral";
    request->send(200, "text/html", html);
  });

  // Handle text & speed update
  server.on("/setText", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("text") && request->hasParam("speed")) {
      displayTextStr = request->getParam("text")->value();
      scrollSpeed = request->getParam("speed")->value().toInt();
      if(scrollSpeed < 10) scrollSpeed = 10; // minimum speed
      P.displayText(displayTextStr.c_str(), PA_CENTER, scrollSpeed, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    }
    request->send(200, "text/plain", "Updated");
  });

  server.begin();
}

void loop() {
  if (P.displayAnimate()) {
    P.displayReset();
  }
}
