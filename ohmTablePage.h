const char ohmTablePage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="sv">
<head>
    <meta charset="UTF-8">
    <title>Ohm Tabell - Temperatur till Resistans</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
    <h1>Simulera Thermistor med Ohm Tabell</h1>
    <button onclick="switchToFormulaPage()">Tillbaka till B-värde Beräkning</button> <!-- Button to switch pages -->

    <!-- Input fields for the Ohm Table -->
    <h2>Ohmtabell för Temperaturer</h2>
    <div>
        <label>-50°C:</label> <input type="number" id="ohm_-50" placeholder="Ohm"><br>
        <label>-40°C:</label> <input type="number" id="ohm_-40" placeholder="Ohm"><br>
        <label>-30°C:</label> <input type="number" id="ohm_-30" placeholder="Ohm"><br>
        <label>-25°C:</label> <input type="number" id="ohm_-25" placeholder="Ohm"><br>
        <label>-20°C:</label> <input type="number" id="ohm_-20" placeholder="Ohm"><br>
        <label>-15°C:</label> <input type="number" id="ohm_-15" placeholder="Ohm"><br>
        <label>-10°C:</label> <input type="number" id="ohm_-10" placeholder="Ohm"><br>
        <label>-5°C:</label> <input type="number" id="ohm_-5" placeholder="Ohm"><br>
        <label>0°C:</label> <input type="number" id="ohm_0" placeholder="Ohm"><br>
        <label>5°C:</label> <input type="number" id="ohm_5" placeholder="Ohm"><br>
        <label>10°C:</label> <input type="number" id="ohm_10" placeholder="Ohm"><br>
        <label>15°C:</label> <input type="number" id="ohm_15" placeholder="Ohm"><br>
        <label>20°C:</label> <input type="number" id="ohm_20" placeholder="Ohm"><br>
        <label>25°C:</label> <input type="number" id="ohm_25" placeholder="Ohm"><br>
        <label>30°C:</label> <input type="number" id="ohm_30" placeholder="Ohm"><br>
        <label>35°C:</label> <input type="number" id="ohm_35" placeholder="Ohm"><br>
        <label>40°C:</label> <input type="number" id="ohm_40" placeholder="Ohm"><br>
        <label>45°C:</label> <input type="number" id="ohm_45" placeholder="Ohm"><br>
        <label>50°C:</label> <input type="number" id="ohm_50" placeholder="Ohm"><br>
        <label>60°C:</label> <input type="number" id="ohm_60" placeholder="Ohm"><br>
        <label>70°C:</label> <input type="number" id="ohm_70" placeholder="Ohm"><br>
        <label>80°C:</label> <input type="number" id="ohm_80" placeholder="Ohm"><br>
        <label>90°C:</label> <input type="number" id="ohm_90" placeholder="Ohm"><br>
    </div>

    <label for="targetTemperature">Mål Temperatur (°C):</label>
    <input type="number" id="targetTemperature" placeholder="Ex: 25">
    <button onclick="calculateAndDisplay()">Beräkna och Visa Graf</button>

    <!-- Display calculated resistance -->
    <h3>Resultat</h3>
    <div id="resultBox">Beräknad Resistans: Ingen data</div>

    <!-- Canvas elements for charts -->
    <h2>Temperatur Graf</h2>
    <canvas id="tempChart" width="400" height="200"></canvas>

    <h2>Resistans Graf</h2>
    <canvas id="resistanceChart" width="400" height="200"></canvas>

<script>
// Function to calculate resistance from temperature using linear interpolation
function calculateResistanceFromTable(targetTemperature) {
    const tempValues = [-50, -40, -30, -25, -20, -15, -10, -5, 0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 60, 70, 80, 90];
    const resistanceValues = [];

    for (let i = -50; i <= 90; i += 5) {
        resistanceValues.push(parseFloat(document.getElementById(`ohm_${i}`).value) || 0);
    }

    // Find the two surrounding temperatures for interpolation
    let lowerIndex = tempValues.findIndex(temp => temp > targetTemperature) - 1;
    let upperIndex = lowerIndex + 1;

    if (lowerIndex < 0 || upperIndex >= tempValues.length) {
        return null; // Temperature out of bounds
    }

    const lowerTemp = tempValues[lowerIndex];
    const upperTemp = tempValues[upperIndex];
    const lowerResistance = resistanceValues[lowerIndex];
    const upperResistance = resistanceValues[upperIndex];

    // Linear interpolation
    const slope = (upperResistance - lowerResistance) / (upperTemp - lowerTemp);
    const resistanceAtTarget = lowerResistance + slope * (targetTemperature - lowerTemp);

    return resistanceAtTarget;
}

// Update function to be called every 5 seconds
function updateSensorData() {
    fetch(`/getTemp`)
        .then(response => response.json())
        .then(data => {
            document.getElementById("currentTemperature").value = data.temperature; // assuming there's an input to show current temperature
            const targetTemperature = parseFloat(document.getElementById("targetTemperature").value);
            if (!isNaN(targetTemperature)) {
                const calculatedResistance = calculateResistanceFromTable(targetTemperature);
                if (calculatedResistance !== null) {
                    document.getElementById("resultBox").innerText = "Beräknad Resistans: " + calculatedResistance.toFixed(2) + " Ω";
                    updateCharts(data.temperature, calculatedResistance);
                }
            }
        })
        .catch(error => {
            console.error("Fel: ", error);
        });
}

// Call updateSensorData every 5000 ms (5 seconds)
setInterval(updateSensorData, 5000);

</script>

</body>
</html>
)rawliteral";