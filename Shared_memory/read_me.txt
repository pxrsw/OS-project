کامپایل برنامه نیازمند فایل های mydefs.h و shm_info.h است. لطفا تمامی این فایل ها را در یک دایرکتوری ذخیره کرده و سپس برنامه های server_shm.c و client_shm.c را کامپایل کنید.

برای کامپایل کردن برنامه های سرور و کلاینت حافظه اشتراکی، میتوان از دستور های زیر استفاده
کرد:
➔ gcc -o server-shm ./server_shm.c -lpthread -lrt
➔ gcc -o client-shm ./client_shm.c -lpthread -lrt
برای اجرا برنامه سرور کافیست server-shm اجرا شود. برای اجرای برنامه کلاینت لازم است پیام
مورد نظر به عنوان آرگومان به برنامه داده شود:
./client-shm text
