
document.addEventListener('DOMContentLoaded', () => {
    updateSoil(); 
    updateLastWatering();
    updateWaterLevel();
    setInterval(updateSoil, 5000);
    setInterval(updateLastWatering, 10000);
    setInterval(updateWaterLevel, 10000);
});

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
    fetch('/get_water_level')
        .then(r => r.text())
        .then(d => {
            const val = parseInt(d) || 0;
            const fill = document.getElementById('waterFill');
            const valText = document.getElementById('waterVal');
            
            fill.style.height = val + "%";
            valText.innerText = val + "%";
            fill.style.background = val < 20 ? "#ffebee" : "#e3f2fd";
        });
}

function startPump() {
    const btn = document.getElementById('pumpBtn');
    btn.disabled = true;
    const originalText = btn.innerText;
    btn.innerText = "PODLEWANIE...";

    fetch('/pump')
        .then(response => {
            if (response.ok) {
                updateLastWatering();
                setTimeout(() => {
                    btn.disabled = false;
                    btn.innerText = originalText;
                }, 5000);
                updateWaterLevel();

            } else {
                throw new Error("Błąd serwera");
            }
        })
        .catch(err => {
            btn.disabled = false;
            btn.innerText = originalText;
            alert("Błąd pompy: " + err.message);
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

function toggleModal() {
    const modal = document.getElementById('settingsModal');
    const isOpening = modal.style.display !== "block";
    modal.style.display = isOpening ? "block" : "none";

    if (isOpening) {
        fetch('/get_tank_config')
            .then(r => r.json())
            .then(data => {
                document.getElementById('distEmpty').value = data.empty;
                document.getElementById('distFull').value = data.full;
            });
    }
}

function saveSettings() {
    const empty = document.getElementById('distEmpty').value;
    const full = document.getElementById('distFull').value;
    
    fetch(`/set_tank?empty=${empty}&full=${full}`)
        .then(r => {
            if(r.ok) {
                alert("Zapisano ustawienia!");
                toggleModal();
                updateWaterLevel();
            }
            else {
                alert("Błąd zapisu ustawień.");
            }
        });
}

window.onclick = (event) => {
    const modal = document.getElementById('settingsModal');
    if (event.target == modal) toggleModal();
};