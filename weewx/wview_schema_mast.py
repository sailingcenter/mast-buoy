"""The MAST Buoy wview schema."""

# =============================================================================
# This is a list containing the default schema of the archive database.  It is
# only used for initialization --- afterwards, the schema is obtained
# dynamically from the database.  Although a type may be listed here, it may
# not necessarily be supported by your weather station hardware.
# =============================================================================
# NB: This schema is specified using the WeeWX V4 "new-style" schema.
# =============================================================================
table = [('dateTime',             'INTEGER NOT NULL UNIQUE PRIMARY KEY'),
         ('usUnits',              'INTEGER NOT NULL'),
         ('interval',             'INTEGER NOT NULL'),
         ('altimeter',            'REAL'),
         ('barometer',            'REAL'),
         ('batteryStatus1',       'REAL'),
         ('batteryStatus2',       'REAL'),
         ('batteryStatus3',       'REAL'),
         ('batteryStatus4',       'REAL'),
         ('batteryStatus5',       'REAL'),
         ('batteryStatus6',       'REAL'),
         ('batteryStatus7',       'REAL'),
         ('batteryStatus8',       'REAL'),
         ('dewpoint',             'REAL'),
         ('ET',                   'REAL'),
         ('extraHumid1',          'REAL'),
         ('extraHumid2',          'REAL'),
         ('extraHumid3',          'REAL'),
         ('extraHumid4',          'REAL'),
         ('extraHumid5',          'REAL'),
         ('extraHumid6',          'REAL'),
         ('extraHumid7',          'REAL'),
         ('extraHumid8',          'REAL'),
         ('extraTemp1',           'REAL'),
         ('extraTemp2',           'REAL'),
         ('extraTemp3',           'REAL'),
         ('extraTemp4',           'REAL'),
         ('extraTemp5',           'REAL'),
         ('extraTemp6',           'REAL'),
         ('extraTemp7',           'REAL'),
         ('extraTemp8',           'REAL'),
         ('heatindex',            'REAL'),
         ('humidex',              'REAL'),
         ('inDewpoint',           'REAL'),
         ('inHumidity',           'REAL'),
         ('inTemp',               'REAL'),
         ('lightning_distance',        'REAL'),
         ('lightning_disturber_count', 'REAL'),
         ('lightning_energy',          'REAL'),
         ('lightning_noise_count',     'REAL'),
         ('lightning_strike_count',    'REAL'),
         ('luminosity',           'REAL'),
         ('maxSolarRad',          'REAL'),
         ('outHumidity',          'REAL'),
         ('outTemp',              'REAL'),
         ('pm10_0',               'REAL'),
         ('pm1_0',                'REAL'),
         ('pm2_5',                'REAL'),
         ('pressure',             'REAL'),
         ('radiation',            'REAL'),
         ('rain',                 'REAL'),
         ('rainRate',             'REAL'),
         ('referenceVoltage',     'REAL'),
         ('rxCheckPercent',       'REAL'),
         ('signal1',              'REAL'),
         ('signal2',              'REAL'),
         ('signal3',              'REAL'),
         ('signal4',              'REAL'),
         ('signal5',              'REAL'),
         ('signal6',              'REAL'),
         ('signal7',              'REAL'),
         ('signal8',              'REAL'),
         ('supplyVoltage',        'REAL'),
         ('UV',                   'REAL'),
         ('windchill',            'REAL'),
         ('windDir',              'REAL'),
         ('windGust',             'REAL'),
         ('windGustDir',          'REAL'),
         ('windSpeed',            'REAL'),
         ]

day_summaries = [(e[0], 'scalar') for e in table
                 if e[0] not in ('dateTime', 'usUnits', 'interval')] + [('wind', 'VECTOR')]

schema = {
    'table': table,
    'day_summaries' : day_summaries
}