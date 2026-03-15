// server.js - EAE API

const express = require("express");
const app = express();
const PORT = process.env.PORT || 3000;

// parse JSON and text
app.use(express.json());

// latest telemetries
let latest = {
    temp: 0,
    avgTemp: 0,
    humidity: 0,
    avgHumidity: 0,
    light: 0,
    avgLight: 0,
    motion: 0,
    action: 0,
    ts: Date.now()
};

// POST endpoint for ESP telemetry
app.post('/api/event', (req, res) => {
    if (!req.body || typeof req.body !== "object") {
        return res.status(400).send("Invalid JSON");
    }

    if(req.body.type !== "heartbeat") {
    latest = { ...latest, ...req.body, ts: Date.now() };
    }
    res.status(200).send("OK");
});

// GET latest telemetry
app.get("/api/data", (req, res) => {
    res.json(latest);
});

// root endpoint
app.get("/", (req, res) => {
    res.send("EAE API running");
});

app.listen(PORT, () => {
    console.log(`EAE API running on port ${PORT}`);
});
