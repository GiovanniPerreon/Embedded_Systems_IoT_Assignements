window.renderWaterLevelGraph = function(history, container) {
    console.log('renderWaterLevelGraph called with history:', history);

    let canvas = container.querySelector('canvas');
    if (canvas) container.removeChild(canvas);
    if (!history || history.length === 0) {
        console.log('No history data available');
        container.textContent = 'No data';
        return;
    }

    canvas = document.createElement('canvas');
    canvas.width = container.offsetWidth || 600;
    canvas.height = 200;
    container.textContent = '';
    container.appendChild(canvas);
    const ctx = canvas.getContext('2d');

    const levels = history.map(r => r.level);
    const min = Math.min(...levels);
    const max = Math.max(...levels);

    ctx.strokeStyle = '#bbb';
    ctx.beginPath();
    ctx.moveTo(40, 10);
    ctx.lineTo(40, 180);
    ctx.lineTo(canvas.width - 10, 180);
    ctx.stroke();

    ctx.strokeStyle = '#2a7ae2';
    ctx.beginPath();
    const N = levels.length;
    for (let i = 0; i < N; i++) {
        const x = N === 1 ? canvas.width / 2 : 40 + (i * (canvas.width - 60) / (N - 1));
        const y = 180 - ((levels[i] - min) / (max - min || 1)) * 160;
        if (i === 0) ctx.moveTo(x, y);
        else ctx.lineTo(x, y);
    }
    ctx.stroke();

    ctx.fillStyle = '#888';
    ctx.font = '12px sans-serif';
    ctx.fillText(max.toFixed(1), 5, 20);
    ctx.fillText(min.toFixed(1), 5, 180);

    ctx.fillStyle = '#2a7ae2';
    ctx.beginPath();
    const lastX = N === 1 ? canvas.width / 2 : 40 + ((N - 1) * (canvas.width - 60) / (N - 1));
    const lastY = 180 - ((levels[N - 1] - min) / (max - min || 1)) * 160;
    ctx.arc(lastX, lastY, 4, 0, 2 * Math.PI);
    ctx.fill();
};
