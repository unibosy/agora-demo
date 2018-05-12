set -x
mkdir ./AgoraPremiumRobot_for_linux_x64
rm -rf ./AgoraPremiumRobot_for_linux_x64/*
cp -f ./AgoraPremiumRobot ./AgoraPremiumRobot_for_linux_x64/
cp -f ./robots_run.py ./AgoraPremiumRobot_for_linux_x64/
cp -f ./config.json ./AgoraPremiumRobot_for_linux_x64/
cp -f ./robots_stop.sh ./AgoraPremiumRobot_for_linux_x64/
cp -f ./README.md ./AgoraPremiumRobot_for_linux_x64/
strip ./AgoraPremiumRobot_for_linux_x64/AgoraPremiumRobot