import weewx
from weewx.engine import StdService

import weewx.units

weewx.units.USUnits['group_dbm'] = 'dBm'
weewx.units.MetricUnits['group_dbm'] = 'dBm'

weewx.units.obs_group_dict['currentDir'] = 'group_direction'
weewx.units.obs_group_dict['currentSpeed'] = 'group_speed'
weewx.units.obs_group_dict['gpsLatitude'] = 'group_angle'
weewx.units.obs_group_dict['gpsLongitude'] = 'group_angle'
weewx.units.obs_group_dict['rxRSSI'] = 'group_dbm'
weewx.units.obs_group_dict['rxSNR'] = 'group_db'
weewx.units.obs_group_dict['waveDir'] = 'group_direction'
weewx.units.obs_group_dict['waveDominantPeriod'] = 'group_deltatime'
weewx.units.obs_group_dict['waveMeanHeight'] = 'group_altitude'
weewx.units.obs_group_dict['waveSignificantHeight'] = 'group_altitude'

class MASTObservations(StdService):
    def __init__(self, engine, config_dict):
        super(MASTObservations, self).__init__(engine, config_dict)