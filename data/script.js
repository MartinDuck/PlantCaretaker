
document.addEventListener('DOMContentLoaded', () => {
    updateSoil(); 
    updateLastWatering();
    updateWaterLevel();
    setInterval(updateSoil, 10000);
    setInterval(updateLastWatering, 10000);
    setInterval(updateWaterLevel, 30000);
    loadConfig();
});

let configMinWater = 10;
let waterLevel = 0;

function loadConfig() {
    fetch('/get_full_config') 
        .then(r => r.json())
        .then(data => {
            configMinWater = data.minWater;
            document.getElementById('minWaterLevel').value = configMinWater;
          
            const autoCheck = document.getElementById('autoModeCheck');
            autoCheck.checked = data.autoMode; 
            sendAutoMode(); 


            document.getElementById('currentThresholdDisplay').innerText = data.threshold;
        });
}

function updateSoil() {
    fetch('/get_soil')
        .then(r => r.text())
        .then(d => {
            const val = parseInt(d) || 0;
            const element = document.getElementById('moisture');
            element.innerText = val + "%";
            element.style.setProperty('--p', val);
            element.style.color = val < 20 ? "#d32f2f" : "#2e7d32";
        });

}

function updateWaterLevel() {

    fetch('/get_water_level', {cache: "no-store"})
        .then(r => r.text())
        .then(d => {
            const val = parseInt(d) || 0;
            waterLevel = val;
            const fill = document.getElementById('waterFill');
            const valText = document.getElementById('waterVal');
           
            const pumpBtn = document.getElementById('pumpBtn'); 
            if (fill && valText) {
                fill.style.height = val + "%";
                valText.innerText = val + "%";
                
        
                if (waterLevel < configMinWater) {
                    fill.style.backgroundColor = "#ee7171"; 
                } else {
                    fill.style.backgroundColor = "#a1c5e3"; 
              

                }
            }
        });
}

function startPump() {
    
    
    const btn = document.getElementById('pumpBtn');

    if (configMinWater > waterLevel) {
        alert("Poziom wody jest zbyt niski, aby uruchomić pompę.");
        return;
    }
    btn.disabled = true;

    const originalText = btn.innerText;
    btn.innerText = "PODLEWANIE...";
    

    fetch('/pump')
        .then(response => {
            if (response.ok) {

                updateLastWatering();
                setTimeout(() => {
                    updateWaterLevel();
                }, 5000);
                setTimeout(() => {
                    btn.disabled = false;
                    btn.innerText = originalText;
                }, 5000);
                
            } else {
                if (response.status === 403) {
                    response.text().then(text => alert(text));
                    btn.disabled = false;
                    btn.innerText = originalText;
                } else {
                    throw new Error("Błąd serwera");
                    
                }
                
            }
        })
        .catch(err => {
            btn.disabled = false;
            btn.innerText = originalText;
            alert("Błąd pompy: " + err.message);
        });
}

function sendAutoMode() {
    const checkbox = document.getElementById('autoModeCheck');
    const label = document.querySelector('.auto-label');
    const isAuto = checkbox.checked;
    const pumpBtn = document.getElementById('pumpBtn');

    label.style.color = isAuto ? "#2196F3" : "#555";

    pumpBtn.disabled = isAuto;
    pumpBtn.style.opacity = isAuto ? "0.5" : "1";

    fetch(`/set_auto?state=${isAuto ? 1 : 0}`);
}

function updateLastWatering() {
    fetch('/last_watering')
        .then(r => r.text())
        .then(timestamp => {
            const ts = parseInt(timestamp);
            const statusElement = document.querySelector('.lastTime');
            if (!statusElement) return;

            if (ts === 0) {
                statusElement.innerText = "Nigdy";
                return;
            }

            const now = Math.floor(Date.now() / 1000);
            const diff = now - ts;

            if (diff < 60) statusElement.innerText = "Przed chwilą";
            else if (diff < 3600) statusElement.innerText = `${Math.floor(diff / 60)} min. temu`;
            else statusElement.innerText = `${Math.floor(diff / 3600)} godz. temu`;
        });
}

function toggleModal() {
    const modal = document.getElementById('settingsModal');
    const isOpening = modal.style.display !== "block";
    modal.style.display = isOpening ? "block" : "none";

    if (isOpening) {
        fetch('/get_full_config')
            .then(r => r.json())
            .then(data => {
                document.getElementById('distEmpty').value = data.empty;
                document.getElementById('distFull').value = data.full;
                document.getElementById('minWaterLevel').value = data.minWater;
            });
    }
}

function saveSettings() {
    const btn = document.querySelector('.save-btn');
    const empty = document.getElementById('distEmpty').value;
    const full = document.getElementById('distFull').value;
    const minWater = document.getElementById('minWaterLevel').value;

    btn.disabled = true;
    const originalText = btn.innerText;
    btn.innerText = "ZAPISYWANIE...";

    fetch(`/set_tank?empty=${empty}&full=${full}&minWater=${minWater}`)
        .then(r => {
            if(r.ok) {
                btn.innerText = "ZAPISANO!";
                btn.style.background = "#2e7d32"; 
                
                configMinWater = parseInt(minWater);
                setTimeout(() => {
                    toggleModal(); 
                    updateWaterLevel();
                    
                    btn.disabled = false;
                    btn.innerText = originalText;
                    btn.style.background = ""; 
                }, 800);
            } else {
                throw new Error("Błąd serwera");
            }
        })
        .catch(err => {
            alert("Błąd: " + err.message);
            btn.disabled = false;
            btn.innerText = originalText;
            btn.style.background = ""; 
        });
}

window.onclick = (event) => {
    const modal = document.getElementById('settingsModal');
    if (event.target == modal) toggleModal();
};

function calibrate(type) {
    const btn = event.currentTarget;
    const originalText = btn.innerText;
    btn.innerText = "..."; 

    fetch('/measure_distance')
        .then(r => r.text())
        .then(val => {
            const dist = parseInt(val);
            if (dist > 0) {
                if (type === 'empty') document.getElementById('distEmpty').value = dist;
                else document.getElementById('distFull').value = dist;
            } else {
                alert("Błąd pomiaru! Sprawdź czujnik.");
            }
            btn.innerText = originalText;
        });
}


function toggleMoistureModal() {
    const modal = document.getElementById('moistureModal');
    modal.style.display = modal.style.display === 'block' ? 'none' : 'block';
    
    if(modal.style.display === 'block') {
        const current = document.getElementById('currentThresholdDisplay').innerText;
        document.getElementById('targetMoistureInput').value = current;
    }
}

function saveMoistureSettings() {
    const newVal = document.getElementById('targetMoistureInput').value;
    const btn = document.querySelector('#moistureModal .save-btn');
    
    btn.innerText = "ZAPISYWANIE...";
    
    fetch(`/set_threshold?val=${newVal}`)
        .then(r => {
            if(r.ok) {
        
                document.getElementById('currentThresholdDisplay').innerText = newVal;

                toggleMoistureModal(); 
                btn.innerText = "ZAPISZ";

            }
            else {
                throw new Error("Błąd serwera");
            }
        })

        .catch(err => {
            alert("Błąd: " + err.message);
            btn.innerText = "ZAPISZ";
        })


}

