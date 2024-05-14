FROM python:3

RUN ln -sf /usr/share/zoneinfo/America/Chicago /etc/localtime

WORKDIR /usr/src/app

COPY requirements.txt ./
RUN pip install --no-cache-dir -r requirements.txt

#COPY .. .

# TODO: The fuzzy-archer skin seems to have a bug w/ unit conversion, it can't properly handle station altitude in feet.
#RUN weectl station create --driver weewx.drivers.simulator --location "MAST Buoy" --altitude '579,foot' --latitude '43.036583' --longitude '-87.846067' --register n --units us
RUN weectl station create --driver weewx.drivers.simulator --location "MAST Buoy" --altitude '176,meter' --latitude '43.036583' --longitude '-87.846067' --register n --units us

COPY wview_schema_mast.py /root/weewx-data/bin/user
COPY mast_obs.py /root/weewx-data/bin/user

RUN cp /root/weewx-data/weewx.conf ./weewx.conf.initial

RUN wget -O weewx-interceptor.zip https://github.com/claudobahn/weewx-interceptor/archive/mast-buoy.zip
RUN weectl extension install weewx-interceptor.zip --yes

RUN weectl station reconfigure --driver user.interceptor --no-prompt

RUN wget -O fuzzy-archerv4.zip https://github.com/claudobahn/fuzzy-archer/archive/refs/heads/mast-buoy.zip
RUN weectl extension install fuzzy-archerv4.zip --yes

RUN cp /root/weewx-data/weewx.conf ./weewx.conf.pre-patch

COPY weewx.conf.patch ./
RUN patch /root/weewx-data/weewx.conf weewx.conf.patch

CMD weewxd
