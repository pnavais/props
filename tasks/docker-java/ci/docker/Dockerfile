FROM feeds.axadmin.net/docker/library/openjdk:8-slim

RUN apt-get update && apt-get install -y \
	apt-transport-https \
	ca-certificates \
	curl \
	gnupg \
	wget \
	supervisor \
	procps \
	--no-install-recommends

RUN curl -L https://raw.githubusercontent.com/tj/n/master/bin/n -o /tmp/n && \
	chmod +x /tmp/n && \
	/tmp/n 10.23.1

RUN apt-get update && apt-get install -y \
    libatk-bridge2.0-0 \
    libgtk-3.0 \
    gconf-service \
    libasound2 \
    libatk1.0-0 \
    libc6 \
    libcairo2 \
    libcups2 \
    libdbus-1-3 \
    libexpat1 \
    libfontconfig1 \
    libgcc1 \
    libgconf-2-4 \
    libgdk-pixbuf2.0-0 \
    libglib2.0-0 \
    libgtk-3-0 \
    libnspr4 \
    libpango-1.0-0 \
    libpangocairo-1.0-0 \
    libstdc++6 \
    libx11-6 \
    libx11-xcb1 \
    libxcb1 \
    libxcomposite1 \
    libxcursor1 \
    libxdamage1 \
    libxext6 \
    libxfixes3 \
    libxi6 \
    libxrandr2 \
    libxrender1 \
    libxss1 \
    libxtst6 \
    libappindicator1 \
    libnss3 \
    lsb-release \
    xdg-utils \
	--no-install-recommends

RUN echo deb http://deb.debian.org/debian stretch main >> /etc/apt/sources.list \
	&& apt-get update && apt-get install -y \
	chromium=73.0.3683.75-1~deb9u1 \
	fontconfig \
    fonts-ipafont-gothic \
    fonts-wqy-zenhei \
    fonts-thai-tlwg \
    fonts-kacst \
    fonts-symbola \
    fonts-noto \
    fonts-roboto \
    fonts-freefont-ttf \
	&& wget --content-disposition -P /usr/share/fonts/truetype/robotomono https://github.com/google/fonts/blob/master/apache/robotomono/static/RobotoMono-Bold.ttf?raw=true \
	&& wget --content-disposition -P /usr/share/fonts/truetype/robotomono https://github.com/google/fonts/blob/master/apache/robotomono/static/RobotoMono-BoldItalic.ttf?raw=true \
	&& wget --content-disposition -P /usr/share/fonts/truetype/robotomono https://github.com/google/fonts/blob/master/apache/robotomono/static/RobotoMono-Italic.ttf?raw=true \
	&& wget --content-disposition -P /usr/share/fonts/truetype/robotomono https://github.com/google/fonts/blob/master/apache/robotomono/static/RobotoMono-Light.ttf?raw=true \
	&& wget --content-disposition -P /usr/share/fonts/truetype/robotomono https://github.com/google/fonts/blob/master/apache/robotomono/static/RobotoMono-LightItalic.ttf?raw=true \
	&& wget --content-disposition -P /usr/share/fonts/truetype/robotomono https://github.com/google/fonts/blob/master/apache/robotomono/static/RobotoMono-Medium.ttf?raw=true \
	&& wget --content-disposition -P /usr/share/fonts/truetype/robotomono https://github.com/google/fonts/blob/master/apache/robotomono/static/RobotoMono-MediumItalic.ttf?raw=true \
	&& wget --content-disposition -P /usr/share/fonts/truetype/robotomono https://github.com/google/fonts/blob/master/apache/robotomono/static/RobotoMono-Regular.ttf?raw=true \
	&& wget --content-disposition -P /usr/share/fonts/truetype/robotomono https://github.com/google/fonts/blob/master/apache/robotomono/static/RobotoMono-Thin.ttf?raw=true \
	&& wget --content-disposition -P /usr/share/fonts/truetype/robotomono https://github.com/google/fonts/blob/master/apache/robotomono/static/RobotoMono-ThinItalic.ttf?raw=true

ARG jar_file
ADD ${jar_file} app.jar
ADD start_java.sh /

RUN mkdir nodeapp
ADD package.json /nodeapp
ADD server.ts /nodeapp
ADD supervisord.conf /etc/supervisor/conf.d/supervisord.conf
RUN cd nodeapp && npm install && npm run tsc server.ts

RUN wget -O /newrelic.jar https://download.newrelic.com/newrelic/java-agent/newrelic-agent/current/newrelic.jar
RUN chmod 755 /start_java.sh

EXPOSE 80:8099

ENTRYPOINT ["/usr/bin/supervisord", "-c", "/etc/supervisor/conf.d/supervisord.conf"]

USER 0

