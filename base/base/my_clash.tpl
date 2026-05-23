port: 7890
socks-port: 7891
allow-lan: true
mode: rule
log-level: info
external-controller: "127.0.0.1:9090"
ipv6: false

dns:
  enable: true
  ipv6: false
  listen: 0.0.0.0:53
  enhanced-mode: fake-ip
  fake-ip-range: 198.18.0.1/16
  use-hosts: true
  fake-ip-filter:
    - "*.lan"
    - "*.local"
    - "*.arpa"
    - time.*.com
    - ntp.*.com
    - +.market.xiaomi.com
    - localhost.ptlogin2.qq.com
    - "*.msftncsi.com"
    - www.msftconnecttest.com
    - +.music.163.com
    - +.music.126.net
    - +.stun.*.*
    - +.stun.*.*.*
    - +.stun.*.*.*.*
    - +.gvt1.com
    - +.cloudflareclient.com
  default-nameserver:
    - 223.5.5.5
    - 119.29.29.29
  nameserver:
    - 223.5.5.5
    - 119.29.29.29
  nameserver-policy:
    "dns.google": [223.5.5.5, 119.29.29.29]
    "cloudflare-dns.com": [223.5.5.5, 119.29.29.29]
  proxy-server-nameserver:
    - 223.5.5.5
    - 119.29.29.29
  fallback:
    - https://1.0.0.1/dns-query
    - https://dns.google/dns-query
  fallback-filter:
    geoip: true
    geoip-code: CN
    geosite:
      - gfw
    ipcidr:
      - 240.0.0.0/4

proxies: ~
proxy-groups: ~
rules: ~
