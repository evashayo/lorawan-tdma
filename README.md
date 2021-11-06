# LoRaWAN TDMA ns-3 module #

- This is addition of TDMA onto LoraWan
- Below are changes are documented in docs/diff-content.html
- Download this repository into src/ folder
- Copy v4 example to scratch
- Copy script folder from examples to root folder (i.e. lorawan-tdma/)
- Run below command: ../waf --run "eva-network-tdma-new-v4 --nDevices=2 --interval=1800 --nHours=2" > mylogs.txt 2>&1 && python ../scripts/tdma.py