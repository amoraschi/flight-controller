function getMedianDelta() {
    return cachedMedianDelta;
}

function detectDrops(records) {
    dropIndices = [];
    if (records.length < 3 || records[0].Tick === undefined) return;
    const deltas = [];
    for (let i = 1; i < records.length; i++) deltas.push(records[i].Tick - records[i - 1].Tick);
    const sorted = deltas.slice().sort((a, b) => a - b);
    const median = sorted[Math.floor(sorted.length / 2)];
    if (median <= 0) return;
    cachedMedianDelta = median;
    const threshold = median * 1.5;
    for (let i = 0; i < deltas.length; i++) {
        if (deltas[i] > threshold) dropIndices.push(i + 1);
    }
}

function decode(buffer) {
    const dv = new DataView(buffer);
    const records = [];
    let pos = 0, skipped = 0;
    while (pos <= buffer.byteLength - RECORD_SIZE) {
        if (dv.getUint16(pos, true) !== SYNC) { pos++; skipped++; continue; }
        const { record, size } = parseRecord(dv, pos);
        records.push(record);
        pos += size;
    }
    return { records, skipped };
}
