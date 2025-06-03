const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>PPG Sensor Dashboard</title>
    <style>
        html { font-family: Arial, Helvetica, sans-serif; text-align: center; }
        h1 { font-size: 1.8rem; color: rgb(250, 250, 239); }
        p { font-size: 1.4rem; }
        .title { background-color: #295a6b; color: white; padding: 10px; }
        body { margin: 0; }
        th, td { text-align: center; padding: 8px; border: 1px solid black; }
        th { background-color: #4db0ac; color: white; }
        table { margin: 0 auto; width: 50%; border: 1px solid black; }
    </style>
</head>
<body>
    <div class="title">
        <h1>Real-Time PPG Sensor Data</h1>
    </div>
    <p>PPG Value: <span id="ppgValue">Waiting...</span></p>
    <canvas id="ppgChart" width="400" height="200"></canvas>

<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
<script>
    var gateway = `ws://${window.location.hostname}/ws`; 
    var websocket;
    var ctx = document.getElementById('ppgChart').getContext('2d');
    
    var ppgChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: [],
            datasets: [{
                label: "PPG Value",
                borderColor: 'red',
                data: [],
                fill: false
            }]
        },
        options: {
            responsive: true,
            animation: false,  // Disable animations for real-time updates
            scales: {
                x: { title: { display: true, text: 'Time' } },
                y: {
                    title: { display: true, text: 'PPG Value' },
                    beginAtZero: false // Allow auto-scaling
                }
            }
        }
    });

function updateChart(ppgValue) {
    let now = new Date().toLocaleTimeString();

    // **Ensure values are within expected range**
    /*if (ppgValue < 1000) {  
        ppgValue += 95000;  // Shift values up for visibility
    }*/

    // Maintain only 30 data points (keeps the graph scrolling)
    if (ppgChart.data.labels.length > 30) { 
        ppgChart.data.labels.shift();
        ppgChart.data.datasets[0].data.shift();
    }

    // Add new data
    ppgChart.data.labels.push(now);
    ppgChart.data.datasets[0].data.push(ppgValue);

    // **Auto-scale Y-axis dynamically**
    let minY = Math.min(...ppgChart.data.datasets[0].data);
    let maxY = Math.max(...ppgChart.data.datasets[0].data);
    let buffer = (maxY - minY) * 0.1; 

    if (isFinite(minY) && isFinite(maxY)) {
        ppgChart.options.scales.y.min = minY - buffer;
        ppgChart.options.scales.y.max = maxY + buffer;
    }

    ppgChart.update();
}

    function initWebSocket() {
        console.log("Opening WebSocket...");
        websocket = new WebSocket(gateway);
        websocket.onopen = () => console.log("WebSocket Connected!");
        websocket.onmessage = function(event) {
            let jsonData = JSON.parse(event.data);
            let ppgValue = parseFloat(jsonData.ppg_value);
            document.getElementById("ppgValue").innerText = ppgValue;
            updateChart(ppgValue);
        };
        websocket.onclose = function() {
            console.log("WebSocket Disconnected! Reconnecting...");
            setTimeout(initWebSocket, 2000);
        };
    }

    window.onload = function() {
        initWebSocket();
    };
</script>
</body>
</html>
)rawliteral";
