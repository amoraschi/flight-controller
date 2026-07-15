const SYNC = 0xCAFE;

const FIELD_DEFS = [
    { name: 'Sync',             type: 'uint16' },
    { name: 'Tick',             type: 'uint32' },
    { name: 'AccelX',           type: 'float32' },
    { name: 'AccelY',           type: 'float32' },
    { name: 'AccelZ',           type: 'float32' },
    { name: 'GyroX',            type: 'float32' },
    { name: 'GyroY',            type: 'float32' },
    { name: 'GyroZ',            type: 'float32' },
    { name: 'MagX',             type: 'float32' },
    { name: 'MagY',             type: 'float32' },
    { name: 'MagZ',             type: 'float32' },
    { name: 'PressurePa',       type: 'float32' },
    { name: 'TemperatureC',     type: 'float32' },
    { name: 'Latitude',         type: 'int32' },
    { name: 'Longitude',        type: 'int32' },
    { name: 'Altitude',         type: 'float32' },
    { name: 'VelX',             type: 'float32' },
    { name: 'VelY',             type: 'float32' },
    { name: 'VelZ',             type: 'float32' },
    { name: 'Flags',            type: 'uint32' },
    { name: 'BatteryVoltage',   type: 'float32' },
    { name: 'LoopExecutionUs',  type: 'uint32' },
    { name: 'LoopMaxExecutionUs', type: 'uint32' },
    { name: 'BMP581ReadUs',     type: 'uint32' },
    { name: 'IIM42653ReadUs',   type: 'uint32' },
    { name: 'IIS2MDCTRReadUs',  type: 'uint32' },
    { name: 'SDWriteUs',        type: 'uint32' },
    { name: 'State',            type: 'uint8' },
    { name: 'SyncEnd',          type: 'uint8' },
];

const TYPE_INFO = {
    uint8:   { size: 1, read: (dv, o) => dv.getUint8(o) },
    uint16:  { size: 2, read: (dv, o) => dv.getUint16(o, true) },
    uint32:  { size: 4, read: (dv, o) => dv.getUint32(o, true) },
    int32:   { size: 4, read: (dv, o) => dv.getInt32(o, true) },
    float32: { size: 4, read: (dv, o) => dv.getFloat32(o, true) },
};

const RECORD_SIZE = FIELD_DEFS.reduce((sum, f) => sum + TYPE_INFO[f.type].size, 0);

function parseRecord(dv, offset) {
    let o = offset;
    const r = {};
    for (const field of FIELD_DEFS) {
        const info = TYPE_INFO[field.type];
        r[field.name] = info.read(dv, o);
        o += info.size;
    }
    return { record: r, size: o - offset };
}
