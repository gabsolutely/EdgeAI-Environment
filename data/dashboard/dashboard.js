// /dashboard.js

const MAX_POINTS = 40;
let autoMode = true;

// cloud address
const isCloud = location.hostname.includes("railway");
const baseURL = isCloud
  ? "https://nodejs-production-41f4.up.railway.app"
  : "";


// chart colros
const COL_TEMP   = "#60a5fa";  // blue
const COL_HUM    = "#10b981";  // green
const COL_LIGHT  = "#facc15";  // yellow
const COL_MOTION = "#fb7185";  // red

let aiEnabled = false; // default, updated at runtime

// toggle AI/math
async function toggleAI() {
  const newValue = aiEnabled ? 0 : 1;

  try {
    const res = await fetch('/ai?enable=' + newValue);
    if (!res.ok) throw new Error('toggle failed');

    const text = await res.text();
    addLog(text);

    aiEnabled = !aiEnabled;
    updateAIBtn();

  } catch (e) {
    addLog("AI toggle error");
    console.error(e);
  }
}

// update AI button
function updateAIBtn() {
  const btn = document.getElementById('aiToggleLink'); // updated

  if (aiEnabled) {
    btn.innerText = "Disable AI";
    btn.classList.remove("danger");
  } else {
    btn.innerText = "Enable AI";
    btn.classList.add("danger");
  }
}

// make chart
function makeChart(ctx, label, color) {
  return new Chart(ctx, {
    type: "line",
    data: {
      labels: [],
      datasets: [{
        label: label,
        data: [],
        borderColor: color,
        backgroundColor: color,
        borderWidth: 2,
        pointRadius: 2,
        pointBackgroundColor: color,
        pointHoverRadius: 4,
        tension: 0.25,
        fill: false
      }]
    },
    options: {
      responsive: true,
      animation: false,
      plugins: {
        legend: { display: true, labels: { color: "#e6eef8" }}
      },
      scales: {
        x: {
          display: false,     // you wanted hidden X axis
          ticks: { color: "#9fb0c8" }
        },
        y: {
          display: true,
          ticks: { color: "#9fb0c8" },
          grid: { color: "rgba(255,255,255,0.05)" }
        }
      }
    }
  });
}

// make motion chart (binary 0/1)
function makeMotionChart(ctx, label) {
  return new Chart(ctx, {
    type: "line",
    data: {
      labels: [],
      datasets: [{
        label: label,
        data: [],
        borderColor: COL_MOTION,
        backgroundColor: COL_MOTION,
        borderWidth: 2,
        pointRadius: 0,
        tension: 0,
        stepped: true
      }]
    },
    options: {
      responsive: true,
      animation: false,
      plugins: {
        legend: { display: true, labels: { color: "#e6eef8" }}
      },
      scales: {
        x: {
          display: false,
          ticks: { color: "#9fb0c8" }
        },
        y: {
          beginAtZero: true,
          max: 1,
          ticks: { stepSize: 1, color: "#9fb0c8" },
          grid: { color: "rgba(255,255,255,0.05)" }
        }
      }
    }
  });
}

// initialize charts
const tempChart   = makeChart(document.getElementById("tempChart").getContext("2d"), 
                              "Temperature (°C)", COL_TEMP);

const humChart    = makeChart(document.getElementById("humChart").getContext("2d"),
                              "Humidity (%)", COL_HUM);

const lightChart  = makeChart(document.getElementById("lightChart").getContext("2d"),
                              "Light (ADC)", COL_LIGHT);

const motionChart = makeMotionChart(document.getElementById("motionChart").getContext("2d"),
                                    "Motion");


// push and trim new data
function pushTrim(chart, label, value){
  chart.data.labels.push(label);
  chart.data.datasets[0].data.push(value);
  if(chart.data.labels.length > MAX_POINTS){
    chart.data.labels.shift();
    chart.data.datasets[0].data.shift();
  }
  chart.update('none');
}

// logging
const logArea = document.getElementById('logArea');
function addLog(s){
  const time = new Date().toLocaleTimeString();
  logArea.innerText = `[${time}] ${s}\n` + logArea.innerText;
  if(logArea.innerText.length > 20000) logArea.innerText = logArea.innerText.slice(0,20000);
}

// fetch and display data
async function fetchData(){
  try{
    let d = null;
    const t = new Date().toLocaleTimeString() + ':' + new Date().getMilliseconds();

    if(isCloud){
      try { d = await (await fetch(baseURL + '/api/data', {cache:'no-store'})).json(); }
      catch(e){
        addLog("Cloud failed → trying local ESP...");
        try { d = await (await fetch("http://" + window.location.hostname + "/data", {cache:'no-store'})).json(); }
        catch(e2){ addLog("Local ESP also failed → no data"); return; }
      }
    } else {
      try { d = await (await fetch("/data", {cache:'no-store'})).json(); }
      catch(e){ addLog("Local fetch failed → no data"); return; }
    }

    // update cards
    document.getElementById('tempVal').innerText = (d.temp ?? 0).toFixed(2) + ' °C';
    document.getElementById('humVal').innerText = Math.round(d.humidity) + ' %';
    document.getElementById('lightVal').innerText = d.light;
    document.getElementById('actionVal').innerText = d.action;
    const motionVal = d.motion ? 1 : 0;
    document.getElementById('motionVal').innerText = motionVal;

    document.getElementById('avgTemp').innerText = 'avg ' + Number(d.avgTemp).toFixed(2);
    document.getElementById('avgHum').innerText = 'avg ' + Math.round(d.avgHumidity);
    document.getElementById('avgLight').innerText = 'avg ' + Number(d.avgLight).toFixed(0);

    // update charts
    pushTrim(tempChart, t, d.temp);
    pushTrim(humChart,  t, d.humidity);
    pushTrim(lightChart,t, d.light);
    pushTrim(motionChart,t, motionVal);

    if(d.action > 0) addLog(`Action ${d.action} active (motion:${motionVal}, L:${d.light})`);
    else addLog(`Idle (T:${d.temp.toFixed(1)} H:${Math.round(d.humidity)} L:${d.light})`);

  }catch(e){
    addLog('Error fetching data');
    console.error(e);
  }
}

// periodic data fetching
setInterval(fetchData, 1000); // every second (adjust as needed)
fetchData();
setTimeout(updateAIBtn, 500);

// manual override 
async function setOverride(value){
  if(isCloud){ addLog("Override disabled in cloud mode"); return; }
  try{
    const res = await fetch('/override?value=' + encodeURIComponent(value));
    if(!res.ok) throw new Error('override failed');
    addLog(`Manual override set to ${value}`);
  }catch(e){ addLog('Override error'); console.error(e); }
}

// csv link
document.getElementById('csvLink').href = baseURL + '/csv';