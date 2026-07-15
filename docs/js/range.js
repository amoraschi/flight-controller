function updateRangeUI() {
    const track = document.getElementById('rangeTrack');
    const tw = track.getBoundingClientRect().width;
    const sel = document.getElementById('rangeSelected');
    const hl = document.getElementById('handleLeft');
    const hr = document.getElementById('handleRight');

    sel.style.left = (viewStart * 100) + '%';
    sel.style.width = ((viewEnd - viewStart) * 100) + '%';
    hl.style.left = (viewStart * tw - 4) + 'px';
    hr.style.left = (viewEnd * tw - 4) + 'px';

    const startIdx = Math.floor(viewStart * allRecords.length);
    const endIdx = Math.floor(viewEnd * allRecords.length);
    document.getElementById('rangeInfo').textContent = `${startIdx} – ${endIdx} (${endIdx - startIdx} samples)`;
}

function initRangeControl() {
    const ctrl = document.getElementById('rangeControl');
    ctrl.classList.remove('hidden');

    const track = document.getElementById('rangeTrack');
    const hl = document.getElementById('handleLeft');
    const hr = document.getElementById('handleRight');
    const sel = document.getElementById('rangeSelected');

    let dragging = null;
    let dragStartX = 0, dragStartLeft = 0, dragStartRight = 0;

    function getPos(e) {
        const rect = track.getBoundingClientRect();
        return Math.max(0, Math.min(1, (e.clientX - rect.left) / rect.width));
    }

    hl.addEventListener('mousedown', (e) => {
        e.preventDefault(); dragging = 'left'; hl.classList.add('active');
    });
    hl.addEventListener('dblclick', (e) => {
        e.preventDefault(); viewStart = 0; updateRangeUI(); redrawAllCharts();
    });
    hr.addEventListener('mousedown', (e) => {
        e.preventDefault(); dragging = 'right'; hr.classList.add('active');
    });
    hr.addEventListener('dblclick', (e) => {
        e.preventDefault(); viewEnd = 1; updateRangeUI(); redrawAllCharts();
    });
    sel.addEventListener('mousedown', (e) => {
        e.preventDefault(); dragging = 'middle';
        dragStartX = e.clientX; dragStartLeft = viewStart; dragStartRight = viewEnd;
    });
    track.addEventListener('mousedown', (e) => {
        if (e.target === track || e.target.tagName === 'CANVAS') {
            const pos = getPos(e);
            const halfSpan = (viewEnd - viewStart) / 2;
            viewStart = Math.max(0, pos - halfSpan);
            viewEnd = Math.min(1, pos + halfSpan);
            if (viewStart < 0) { viewEnd -= viewStart; viewStart = 0; }
            if (viewEnd > 1) { viewStart -= (viewEnd - 1); viewEnd = 1; }
            updateRangeUI(); redrawAllCharts();
        }
    });

    document.addEventListener('mousemove', (e) => {
        if (!dragging) return;
        const pos = getPos(e);
        if (dragging === 'left') {
            viewStart = Math.min(pos, viewEnd - 0.01);
        } else if (dragging === 'right') {
            viewEnd = Math.max(pos, viewStart + 0.01);
        } else if (dragging === 'middle') {
            const rect = track.getBoundingClientRect();
            const dx = (e.clientX - dragStartX) / rect.width;
            const span = dragStartRight - dragStartLeft;
            let newStart = dragStartLeft + dx;
            let newEnd = dragStartRight + dx;
            if (newStart < 0) { newStart = 0; newEnd = span; }
            if (newEnd > 1) { newEnd = 1; newStart = 1 - span; }
            viewStart = newStart; viewEnd = newEnd;
        }
        updateRangeUI(); redrawAllCharts();
    });

    document.addEventListener('mouseup', () => {
        dragging = null;
        hl.classList.remove('active'); hr.classList.remove('active');
    });

    drawMinimap();
    updateRangeUI();
}
