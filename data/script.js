document.addEventListener('DOMContentLoaded', () => {
    loadConfig();
    
    updateSoil(); 
    updateLastWatering();
    updateWaterLevel();
    
    setInterval(updateSoil, 10000);
    setInterval(updateLastWatering, 10000);
    setInterval(updateWaterLevel, 30000); 
});

let configMinWater = 10;
let currentWaterLevel = 0;
let isAutoMode = false;
let isPumping = false;

function updateButtonState() {
    const btn = document.getElementById('pumpBtn');
    if (!btn) return;

    if (isPumping) {
        btn.disabled = true;
        btn.innerText = "PODLEWANIE...";
        btn.style.opacity = "1";
        return;
    }

    if (currentWaterLevel < configMinWater) {
        btn.disabled = true;
        btn.innerText = "BRAK WODY";
        btn.style.opacity = "0.6";
        btn.style.cursor = "not-allowed";
        return;
    }

    if (isAutoMode) {
        btn.disabled = true;
        btn.innerText = "TRYB AUTO";
        btn.style.opacity = "0.6";
        btn.style.cursor = "not-allowed";
        return;
    }

    btn.disabled = false;
    btn.innerText = "PODLEJ RĘCZNIE";
    btn.style.opacity = "1";
    btn.style.cursor = "pointer";
}

function loadConfig() {
    fetch('/get_full_config') 
        .then(r => r.json())
        .then(data => {
         
            configMinWater = data.minWater;
            isAutoMode = data.autoMode;
            
            document.getElementById('minWaterLevel').value = configMinWater;
            document.getElementById('autoModeCheck').checked = isAutoMode;
            document.getElementById('currentThresholdDisplay').innerText = data.threshold;
         
            const label = document.querySelector('.auto-label');
            if(label) label.style.color = isAutoMode ? "#2196F3" : "#555";

            updateButtonState();
        })
        .catch(err => console.error("Błąd konfiguracji:", err));
}

function updateSoil() {
    fetch('/get_soil')
        .then(r => r.text())
        .then(d => {
            const val = parseInt(d) || 0;
            const element = document.getElementById('moisture');
            if (element) {
                element.innerText = val + "%";
                element.style.setProperty('--p', val); 
                
                element.style.color = val < 20 ? "#d32f2f" : "#2e7d32";
            }
        });
}

function updateWaterLevel() {
    fetch('/get_water_level', {cache: "no-store"})
        .then(r => r.text())
        .then(d => {
            const val = parseInt(d) || 0;
            currentWaterLevel = val; 
            
            const fill = document.getElementById('waterFill');
            const valText = document.getElementById('waterVal');
            
            if (fill && valText) {
                fill.style.height = val + "%";
                valText.innerText = val + "%";
             
                if (val < configMinWater) {
                    fill.style.backgroundColor = "#ee6666"; 
                } else {
                    fill.style.backgroundColor = "#59a4e0"; 
                }
            }
         
            updateButtonState();
        });
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


function startPump() {
    if (currentWaterLevel < configMinWater) {
        alert("Poziom wody jest zbyt niski!");
        return;
    }

    isPumping = true;
    updateButtonState(); 

    fetch('/pump')
        .then(response => {
            if (response.ok) {
                updateLastWatering();
                
                setTimeout(updateWaterLevel, 6000); 
        
                setTimeout(() => {
                    isPumping = false;
                    updateButtonState();
                }, 7000); 
                
            } else {
                if (response.status === 403) {
                    response.text().then(text => alert(text));
                } else {
                    throw new Error("Błąd serwera");
                }
                isPumping = false;
                updateButtonState();
            }
        })
        .catch(err => {
            alert("Błąd połączenia: " + err.message);
            isPumping = false;
            updateButtonState();
        });
}

function sendAutoMode() {
    const checkbox = document.getElementById('autoModeCheck');
    isAutoMode = checkbox.checked; 
    
    const label = document.querySelector('.auto-label');
    if(label) label.style.color = isAutoMode ? "#2196F3" : "#555";

    updateButtonState(); 

    fetch(`/set_auto?state=${isAutoMode ? 1 : 0}`);
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

function toggleMoistureModal() {
    const modal = document.getElementById('moistureModal');
    const isOpening = modal.style.display !== "block";
    modal.style.display = isOpening ? "block" : "none";
    
    if(isOpening) {
        const current = document.getElementById('currentThresholdDisplay').innerText;
        document.getElementById('targetMoistureInput').value = current;
    }
}

window.onclick = (event) => {
    if (event.target.classList.contains('modal')) {
        event.target.style.display = "none";
    }
};

function saveSettings() {
    const btn = document.querySelector('#settingsModal .save-btn');
    const empty = document.getElementById('distEmpty').value;
    const full = document.getElementById('distFull').value;
    const minWater = document.getElementById('minWaterLevel').value;

    btn.disabled = true;
    btn.innerText = "ZAPISYWANIE...";

    fetch(`/set_tank?empty=${empty}&full=${full}&minWater=${minWater}`)
        .then(r => {
            if(r.ok) {
                btn.innerText = "ZAPISANO!";
                btn.style.background = "#2e7d32"; 
                
                configMinWater = parseInt(minWater);
                
                setTimeout(() => {
                    document.getElementById('settingsModal').style.display = "none"; 
                    updateWaterLevel(); 
            
                    btn.disabled = false;
                    btn.innerText = "ZAPISZ";
                    btn.style.background = ""; 
                }, 800);
            } else {
                throw new Error("Błąd danych");
            }
        })
        .catch(err => {
            alert("Błąd: " + err.message);
            btn.disabled = false;
            btn.innerText = "ZAPISZ";
        });
}

function saveMoistureSettings() {
    const newVal = document.getElementById('targetMoistureInput').value;
    const btn = document.querySelector('#moistureModal .save-btn');
    
    btn.innerText = "ZAPISYWANIE...";
    
    fetch(`/set_threshold?val=${newVal}`)
        .then(r => {
            if(r.ok) {
                document.getElementById('currentThresholdDisplay').innerText = newVal;
                document.getElementById('moistureModal').style.display = "none";
                btn.innerText = "ZAPISZ";
            } else {
                throw new Error("Błąd serwera");
            }
        })
        .catch(err => {
            alert("Błąd: " + err.message);
            btn.innerText = "ZAPISZ";
        });
}

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
                alert("Błąd odczytu z czujnika!");
            }
            btn.innerText = originalText;
        });
}