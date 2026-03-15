EAE - Cloud / Dashboard docs 
-------------------------------
Static dashboard + example API calls to monitor device metrics, see averages, manual override outputs, and download CSV logs.

# Frontend endpoints (expected on the device / CloudManager)
> **GET /data**
  - Response (JSON example):
    {
      "temp": 31.4,
      "avgTemp": 30.8,
      "humidity": 79.0,
      "avgHumidity": 78.5,
      "light": 3600,
      "avgLight": 2900,
      "motion": 0,
      "action": 0
    }
  - Called by dashboard every second.

> **GET /csv**
  - Returns CSV file with headers like:
    `ts,temp,avgTemp,hum,avgHum,light,avgLight,motion,action`
  - Dashboard links to this for download.

> **GET /override?value=<int>**
  - Sets manual override on the device.
  - `value` definitions:
    - `-1` - return to auto (AI)
    - `0` - no action
    - `1` - LED1
    - `2` - LED2
    - `3` - LED3/Buzzer
  - (If your CloudManager prefers POST, use accept JSON `{ "value": N }`)

---------------------------------------

# CloudManager integration ideas
>  Option A — **Device-hosted dashboard**  
  - Keep static files on ESP SPIFFS and serve `/dashboard.html`, which is good for local demos.

>  Option B — **Remote-hosted dashboard** (This projects version)
  - Host `/cloud` on Railway / Glitch / Replit, etc. Devices push telemetry to a small HTTP endpoint or MQTT broker.
  - Flowchart for noobs (ESP32 -> HTTP POST -> Cloud API -> DB/cache -> /data -> Dashboard)

>  Option C — **MQTT + Broker** (Optional upgrade)
  - Device publishes `devices/<id>/telemetry`.
  - Dashboard subscribes (using a lightweight server proxy or WebSocket) for low-latency updates.

# Security & improvements
  - Add basic auth or token to override endpoint before public hosting.
  - Use TLS if sending data to remote cloud.
  - Use WebSockets for real-time push to dashboard (no polling).
  - Add rate-limiting & authentication on override endpoint.

# Next steps (future)
  - Add dashboard controls for firmware update triggers.
  - Add per-device selector (if multiple devices/gateways).
  - Persist last N days to lightweight DB or cloud storage (S3/folder) for long-term analytics.