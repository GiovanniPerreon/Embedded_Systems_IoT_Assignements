const API_PORT = window.CONFIG_HTTP_PORT;
const API_BASE = window.CONFIG_API_BASE;
const DOMUpdateRate = window.CONFIG_DOM_UPDATE_RATE;

let currentMode = 'NOT AVAILABLE';
let lastRequestedMode = 'AUTOMATIC';
let isAvailable = false;

const stateEl = document.querySelector('.state');
const valveEl = document.querySelector('.valve');
const modeBtn = document.querySelector('.mode-btn');
const valveSlider = document.getElementById('valve-slider');
const graphPlaceholder = document.querySelector('.graph-placeholder');

setDashboardUnavailable();

document.addEventListener('DOMContentLoaded', () => {
    fetchState();
    setInterval(fetchState, DOMUpdateRate);
});

async function fetchState() {
    try {
        const res = await fetch(`${API_BASE}/state`);
        if (!res.ok) throw new Error('HTTP error');
        const data = await res.json();
        updateDashboard(data);
        isAvailable = true;
    } catch (err) {
        setDashboardUnavailable();
        isAvailable = false;
    }
}

async function setMode(mode) {
    try {
        const res = await fetch(`${API_BASE}/mode`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ mode })
        });
        return await res.json();
    } catch (err) {
        return { error: 'Network error' };
    }
}

async function setValve(percent) {
    try {
        const res = await fetch(`${API_BASE}/valve`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ percent })
        });
        return await res.json();
    } catch (err) {
        return { error: 'Network error' };
    }
}

function updateDashboard(data) {
    modeBtn.disabled = true;
    valveSlider.disabled = true;
    if (data.mode === 'UNCONNECTED') {
        stateEl.textContent = 'UNCONNECTED';
        valveEl.textContent = '--';
        currentMode = 'UNCONNECTED';
    } else if (data.mode === 'NOT AVAILABLE') {
        setDashboardUnavailable();
        return;
    } else {
        stateEl.textContent = data.mode;
        valveEl.textContent = data.valveOpenPercent + '%';
        currentMode = data.mode;
        modeBtn.disabled = false;
        // Only enable slider if system is in MANUAL mode (from backend!)
        if (data.mode === 'MANUAL') {
            valveSlider.disabled = false;
        }
    }
    modeBtn.textContent = (currentMode === 'MANUAL') ? 'Switch to AUTOMATIC' : 'Switch to MANUAL';
    valveSlider.value = data.valveOpenPercent;
    if (window.renderWaterLevelGraph) {
        window.renderWaterLevelGraph(data.waterLevelHistory, graphPlaceholder);
    } else {
        graphPlaceholder.textContent = 'no graph data available';
    }
}

function setDashboardUnavailable() {
    modeBtn.disabled = true;
    valveSlider.disabled = true;
    stateEl.textContent = 'NOT AVAILABLE';
    valveEl.textContent = '--';
    graphPlaceholder.textContent = 'No data';
    currentMode = 'NOT AVAILABLE';
}

modeBtn.addEventListener('click', async () => {
    if (!isAvailable || currentMode === 'UNCONNECTED' || currentMode === 'NOT AVAILABLE') {
        alert(currentMode === 'NOT AVAILABLE' ? 'Cannot switch mode: backend not available' : 'Cannot switch mode: system is UNCONNECTED');
        return;
    }
    const newMode = lastRequestedMode === 'MANUAL' ? 'AUTOMATIC' : 'MANUAL';
    lastRequestedMode = newMode;
    const result = await setMode(newMode);
    if (result.success) {
        fetchState();
    } else {
        alert(result.error || 'Failed to change mode');
    }
});

valveSlider.addEventListener('input', async (e) => {
    if (!isAvailable || currentMode !== 'MANUAL' || currentMode === 'UNCONNECTED' || currentMode === 'NOT AVAILABLE') {
        alert(currentMode === 'NOT AVAILABLE' ? 'Cannot set valve: backend not available' : 'Cannot set valve: system is UNCONNECTED or not in MANUAL mode');
        return;
    }
    const percent = parseInt(e.target.value, 10);
    const result = await setValve(percent);
    if (!result.success) {
        alert(result.error || 'Failed to set valve');
    }
    // Always fetch state after setting, to sync with backend
    fetchState();
});
