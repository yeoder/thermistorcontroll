const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="sv">
<head>
    <meta charset="UTF-8">
    <title>Simulera Termistor – Temperatur till Resistans</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
    <h1>Simulera Termistor – Temperatur till Resistans</h1>
    
    <div>
        <label for="currentTemperature">Aktuell Temperatur (°C):</label>
        <input type="number" id="currentTemperature" placeholder="Aktuell Temperatur" readonly><br>
        
        <label for="resistanceAt25">Resistans vid 25 °C (Ω):</label>
        <input type="number" id="resistanceAt25" placeholder="Ex: 4700" required><br>
        
        <label for="thermistorType">Termistortyp:</label>
        <input type="text" id="thermistorType" value="NTC" readonly><br>
        
        <label for="bValue">B-värde (K):</label>
        <input type="number" id="bValue" placeholder="Ex: 3977" required><br>
        
        <label for="bValueTolerance">B-värdestolerans (±):</label>
        <input type="number" id="bValueTolerance" placeholder="Ex: 0.75" required><br>
        
        <button onclick="calculateAndDisplay()">Beräkna och Visa Graf</button>
    </div>

    <h3>Resultat</h3>
    <div id="resultBox">Beräknad Resistans: Ingen data</div>

    <h2>Temperatur Graf</h2>
    <canvas id="tempChart" width="400" height="200"></canvas>

    <h2>Resistans Graf</h2>
    <canvas id="resistanceChart" width="400" height="200"></canvas>

    <script>
        const maxDataPoints = 40; // Maximum points to display
        let currentTemperature = 0; // Spara aktuell temperatur
        let resistanceAt25 = 0; // Spara resistans vid 25 °C
        let bValue = 0; // Spara B-värde

        // Temperature Chart
        const tempChartCtx = document.getElementById('tempChart').getContext('2d');
        const tempChart = new Chart(tempChartCtx, {
            type: 'line',
            data: {
                labels: [],
                datasets: [{
                    label: 'Temperatur °C',
                    data: [],
                    borderColor: 'blue',
                    borderWidth: 2,
                    fill: false,
                }]
            },
            options: {
                scales: {
                    x: { type: 'linear', position: 'bottom' },
                    y: { beginAtZero: true, title: { display: true, text: 'Temperatur (°C)' }}
                }
            }
        });

        // Resistance Chart
        const resistanceChartCtx = document.getElementById('resistanceChart').getContext('2d');
        const resistanceChart = new Chart(resistanceChartCtx, {
            type: 'line',
            data: {
                labels: [],
                datasets: [{
                    label: 'Resistans (Ω)',
                    data: [],
                    borderColor: 'red',
                    borderWidth: 2,
                    fill: false,
                }]
            },
            options: {
                scales: {
                    x: { type: 'linear', position: 'bottom' },
                    y: { beginAtZero: true, title: { display: true, text: 'Resistans (Ω)' }}
                }
            }
        });

        function calculateAndDisplay() {
            const userTemperature = parseFloat(document.getElementById("currentTemperature").value) || null;
            let currentTemperature = userTemperature;

            // Om användaren inte har fyllt i något, hämta den aktuella temperaturen från servern
            if (currentTemperature === null) {
                fetchCurrentTemperature().then(fetchedTemperature => {
                    if (fetchedTemperature !== null) {
                        currentTemperature = fetchedTemperature; // Använd hämtad temperatur
                        performCalculation(currentTemperature); // Utför beräkning
                    } else {
                        document.getElementById("resultBox").innerText = "Kunde inte hämta aktuell temperatur.";
                    }
                });
            } else {
                performCalculation(currentTemperature); // Utför beräkning om användarens temperatur används
            }
        }

        function performCalculation(currentTemperature) {
            const resistanceAt25 = parseFloat(document.getElementById("resistanceAt25").value);
            const bValue = parseFloat(document.getElementById("bValue").value);

            if (isNaN(resistanceAt25) || isNaN(bValue)) {
                document.getElementById("resultBox").innerText = "Vänligen fyll i alla fält korrekt.";
                return;
            }

            fetch(`/calculateResistance?temperature=${currentTemperature}&R25=${resistanceAt25}&B_value=${bValue}`)
            .then(response => {
                if (!response.ok) {
                    return response.json().then(errorData => {
                        throw new Error(errorData.error || "Okänt fel");
                    });
                }
                return response.json();
            })
            .then(data => {
                document.getElementById("resultBox").innerText = "Beräknad Resistans: " + data.calculatedOhm + " Ω";
                updateCharts(currentTemperature, data.calculatedOhm);
            })
            .catch(error => {
                document.getElementById("resultBox").innerText = "Fel: " + error.message;
            });
        }

        function updateCharts(temp, resistance) {
            if (tempChart.data.labels.length >= maxDataPoints) {
                tempChart.data.labels.shift(); // Ta bort den äldsta etiketten
                tempChart.data.datasets[0].data.shift(); // Ta bort den äldsta datan
            }
            tempChart.data.labels.push(tempChart.data.labels.length + 1); // Lägg till ny etikett
            tempChart.data.datasets[0].data.push(temp); // Lägg till temperaturdata

            if (resistanceChart.data.labels.length >= maxDataPoints) {
                resistanceChart.data.labels.shift(); // Ta bort den äldsta etiketten
                resistanceChart.data.datasets[0].data.shift(); // Ta bort den äldsta datan
            }
            resistanceChart.data.labels.push(resistanceChart.data.labels.length + 1); // Lägg till ny etikett
            resistanceChart.data.datasets[0].data.push(resistance); // Lägg till resistansdata

            // Uppdatera graferna
            tempChart.update();
            resistanceChart.update();
        }
       // Funktion för att hämta aktuell temperatur från servern
        async function fetchCurrentTemperature() {
           try {
              const response = await fetch(`/getTemp`);
                if (!response.ok) {
                   throw new Error("Nätverksrespons var inte OK");
                }
                const data = await response.json();

               // Kontrollera att data innehåller rätt fält
               if (data.temperature !== undefined && data.resistance !== undefined) {
                   document.getElementById("currentTemperature").value = data.temperature; // Uppdatera aktuellt temperaturfält
                    return data.temperature; // Returnera den hämtade temperaturen
                } else {
                    console.error("Ogiltig data från servern: ", data);
                    return null; // Returnera null om datan är ogiltig
               }
            } catch (error) {
               console.error("Fel: ", error);
                return null; // Returnera null vid fel
            }
          }

        // Anropa funktionen för att hämta aktuell temperatur varje 5 sekunder
        setInterval(() => {
           fetchCurrentTemperature().then(fetchedTemperature => {
               if (fetchedTemperature !== null) {
                   updateCharts(fetchedTemperature, resistanceAt25); // Använd den hämtade temperaturen
               }
           });
          }, 5000);
</script>

  <!-- Button to navigate to the Ohm Table Page -->
  <button onclick="location.href='/ohmTablePage'">Ohm Tabell</button>
</body>
</html>
)rawliteral";

