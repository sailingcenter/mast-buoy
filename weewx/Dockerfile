FROM python:3

WORKDIR /usr/src/app

COPY requirements.txt ./
RUN pip install --no-cache-dir -r requirements.txt

#COPY .. .

RUN weectl station create --driver weewx.drivers.simulator --location "MAST Buoy" --altitude '579,foot' --latitude '43.036583' --longitude '-87.846067' --register n --units us

RUN cp /root/weewx-data/weewx.conf ./weewx.conf.initial

RUN wget -O weewx-interceptor.zip https://github.com/matthewwall/weewx-interceptor/archive/master.zip
RUN weectl extension install weewx-interceptor.zip --yes

RUN weectl station reconfigure --driver user.interceptor --no-prompt

RUN wget -O fuzzy-archerv4.zip https://github.com/brewster76/fuzzy-archer/archive/refs/heads/master.zip
RUN weectl extension install fuzzy-archerv4.zip --yes

RUN cp /root/weewx-data/weewx.conf ./weewx.conf.pre-patch

COPY weewx.conf.patch ./
RUN patch /root/weewx-data/weewx.conf weewx.conf.patch

CMD weewxd
