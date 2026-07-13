function processFile(file) {
    document.getElementById('fileInfo').textContent = `${file.name} (${(file.size / 1024).toFixed(1)} KB)`;
    const reader = new FileReader();
    reader.onload = function(e) {
        const { records, skipped } = decode(e.target.result);
        if (!records.length) { alert('No valid records. Check format.'); return; }
        allRecords = records;
        detectDrops(records);
        viewStart = 0; viewEnd = 1;
        document.getElementById('dropZone').classList.add('hidden');
        renderStats(records, skipped);
        renderCharts();
        initRangeControl();
    };
    reader.readAsArrayBuffer(file);
}

const dropZone = document.getElementById('dropZone');
const fileInput = document.getElementById('fileInput');
dropZone.addEventListener('click', () => fileInput.click());
dropZone.addEventListener('dragover', (e) => { e.preventDefault(); dropZone.classList.add('dragover'); });
dropZone.addEventListener('dragleave', () => dropZone.classList.remove('dragover'));
dropZone.addEventListener('drop', (e) => {
    e.preventDefault(); dropZone.classList.remove('dragover');
    if (e.dataTransfer.files.length) processFile(e.dataTransfer.files[0]);
});
fileInput.addEventListener('change', (e) => { if (e.target.files.length) processFile(e.target.files[0]); });

window.addEventListener('resize', () => {
    if (allRecords.length) { redrawAllCharts(); drawMinimap(); updateRangeUI(); }
});
