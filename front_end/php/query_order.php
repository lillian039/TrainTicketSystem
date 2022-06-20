<?php
    header("content-type:text/html;charset=utf-8");
    error_reporting(E_ALL &~E_NOTICE);
    $id = $_POST["username"];
    $password = $_POST["password"];
    $command = "query_order ".$id." ".$password;
    $socket = socket_create(AF_INET,SOCK_STREAM,SOL_TCP);
    socket_connect($socket,"81.68.199.253",443);
    socket_write($socket,$command);
    $tmp_res=socket_read($socket,100000);
    // setcookie("username", $id, time()+3600);
    echo $tmp_res;
?>