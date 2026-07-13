function getCol(records, key) {
    return records.map(r => r[key]);
}

function arrMin(a) {
    let m = a[0];
    for (let i = 1; i < a.length; i++) if (a[i] < m) m = a[i];
    return m;
}

function arrMax(a) {
    let m = a[0];
    for (let i = 1; i < a.length; i++) if (a[i] > m) m = a[i];
    return m;
}

function formatNum(v) {
    if (Math.abs(v) >= 1000) return v.toFixed(0);
    if (Math.abs(v) >= 1) return v.toFixed(2);
    return v.toFixed(4);
}
