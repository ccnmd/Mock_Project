cmd_/home/duong/Mock_project1/btn_driver.mod := printf '%s\n'   btn_driver.o | awk '!x[$$0]++ { print("/home/duong/Mock_project1/"$$0) }' > /home/duong/Mock_project1/btn_driver.mod
