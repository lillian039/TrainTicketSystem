<?php
    header("content-type:text/html;charset=utf-8");
    error_reporting(E_ALL &~E_NOTICE);
    $id = $_POST["id"];
    $name = $_POST["name"];
    $password = $_POST["password"];
    $email = $_POST["email"];
    $password_pre = $_POST["password_pre"];
    $command = "modify_profile -u ".$id." -p ".$password." -n ".$name." -m ".$email." -g ".$password_pre;
    $socket = socket_create(AF_INET,SOCK_STREAM,SOL_TCP);
    socket_connect($socket,"81.68.199.253",443);
    socket_write($socket,$command);
    $tmp_res=socket_read($socket,2048);
    echo "<script>alert('$tmp_res')</script>";
    echo "<script language='javascript' type='text/javascript'>";
    echo "window.location.href='http://81.68.199.253/index.html'";
    echo "</script>";
?>