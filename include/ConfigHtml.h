// MyLibrary/Constants.h
#ifndef CONFIGHTML_H
#define CONFIGHTML_H

namespace ConfigHtml {
    
    String initHtml = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Wifi Scan</title>
</head>
<style>
    * {
        margin: 0;
        padding: 0;
        color: rgb(82, 82, 82);
        font-family: 'Microsoft YaHei', Arial, sans-serif;
        font-size: 3rem;
    }

    body {
        position: fixed;
        margin: 0;
        padding: 0;
        display: flex;
        flex-direction: column;
        justify-content: center;
        align-items: center;
        height: 100vh;
        width: 100vw;
        opacity: 1;
    }    
    .main {
        display: flex;
        flex-direction: column;    /* 垂直排列 */
        justify-content: center;  /* 居中 */
        align-items: center;     /* 居中 */
        height: 100rem;
        width: 80rem;
    }

    .top {
        user-select: none;
        font-size: 4rem;
        position: relative;  
        width: 100%;
        height: 15rem;
        display: flex;
        justify-content: center;
        align-items: center;
        flex-direction: row;
        background-color: #eeeeee;
        border-radius: 20px;
        box-shadow: 5px 5px 10px rgba(0, 0, 0, 0.5);
    }
    .inner {  /* wifi列表 */
        margin-top: 1rem;  /* 距离顶部的距离 */
        padding: 2.5rem 0rem;
        top: 0;
        left: 0;
        width: 100%;
        height: 100%;
        border-radius: 20px;
        background-color: #eeeeee;
        box-shadow: 5px 5px 10px rgba(0, 0, 0, 0.5);
        z-index: 1;
        overflow-y: auto;
        scrollbar-width: none;
        outline: 2rem solid #eeeeee;   
        outline-offset: -2rem;
    }

    .list_item {  /* wifi列表项 */
        user-select: none;
        display: flex;
        justify-content: center;
        align-items: center;
        margin: 2rem 5rem;
        height: 15rem;
        font-size: 3rem;
        font-weight: bold;
        border-radius: 2rem;
        background-color: rgb(255, 255, 255);
    }
    .list-item:hover {
        background-color: #727272;
    }
    .list_item_error {  /*未找到网络连接*/
        user-select: none;
        text-align: center;
        margin: 50% 5px;
    }


    .tail {
        user-select: none;
        bottom: 0;  
        width: 100%;
        height: 10rem;
        display: flex;
        justify-content: center;
        align-items: center;
        font-size: 2rem;
        color: rgb(82, 82, 82);
    }
    .bg {
        pointer-events: none;  /* 禁止点击背景 */
        user-select: none;    /* 禁止选中文本 */
        height: 10%;
        width: 200%;
        display: flex;
        flex-direction: column;
        position: absolute;  
        justify-items: center;  
        align-items: center;  
        z-index: 1001;
        transform: rotate(0deg);
    }

    .bg-text {
        user-select: none;
        font-weight: 700;
        font-size: 15rem;
        color: rgba(116, 116, 116, 0.5);
    }

    #modal-overlay {
        user-select: none;
        position: fixed;
        top: 0;
        left: 0;
        width: 100%;
        height: 100%;
        background-color: rgba(141, 141, 141, 0.5);
        z-index: 999;
        display: none;
        backdrop-filter: blur(10px);
    }
    #modal {
        user-select: none;
        display: none;
        position: fixed;
        flex-direction: column;
        top: 50%;
        left: 50%;
        transform: translate(-50%, -50%);
        width: 60rem;
        padding: 3rem 5rem 1rem 5rem;
        background-color: white;
        box-shadow: 5px 5px 5px rgba(0, 0, 0, 0.5);
        border-radius: 8px;
        z-index: 1000;
    }
    #item-content {
        user-select: none;
        font-size: 3rem;
        font-weight: bold;
        margin-bottom: 1.5rem;
    }
    .item-inners {
        display: flex;  
        justify-content: center;
        align-items: center;
    }
    #modal input {
        box-sizing: border-box;
        height: 6rem;
        width: 100%;
        padding: 1rem;
        border-radius: 5px;
        border: none;
        box-shadow: 2px 2px 5px rgba(0, 0, 0, 0.5);
    }
    #modal button {
        user-select: none;
        vertical-align: top;  /* 垂直居中 */
        height: 6rem;
        width: 20%;
        margin-left: 5%;
        border-radius: 5px;
        border: none;
        background-color:black;
        color: white;
        cursor: pointer;
        box-shadow: 2px 2px 5px rgba(0, 0, 0, 0.5);
    }
    #modal button:hover {
        background-color: white;
        color: black;
    }
    #item-error-message {
        user-select: none;
        color: red;
        font-size: 2rem;
        margin-top: 1.5rem;
        opacity: 0;
        transition: opacity 0.5s ease;
    }
    #loading-ring-overlay {
        user-select: none;
        display: none;
        justify-content: center;
        align-items: center;
        position: fixed;
        top: 0;
        left: 0;
        width: 100%;
        height: 100%;
        z-index: 1002;
        background-color: rgba(255, 255, 255, 0.1);
        backdrop-filter: blur(10px);
    }

    #loading-ring {
        user-select: none;
        position: absolute;
        width: 10rem;
        height: 10rem;
        border-radius: 50%;
        border: 5px solid transparent;
        border-top: 4px solid #696969;  
        border-bottom: 4px solid #696969;
        animation: rotate 2s cubic-bezier(0,1,1,0) infinite;
        z-index: 1002;
    }
    @keyframes rotate {
        0% {
            transform: rotate(0deg);
        }
        100% {
            transform: rotate(360deg);
        }
    }

    
</style>
<body>
    <div id="loading-ring-overlay">
        <div id="loading-ring"></div>
    </div>
    <div class="bg"><p class="bg-text">JAHSEHKYLE KUIBOYANG</p></div> <!-- 水印文字 -->
    <div id="modal-overlay"></div>
    <div id="modal">
        <div id="item-content">SSID: xxxxxx</div>
        <div class ="item-inners">
            <input type="password" id="password-input" placeholder="请输入密码">
            <button id="submit-button">取消</button>
        </div>
        <div id="item-error-message">"响应超时，请重新输入！"</div>
    </div>
    <div class="main">
        <div class="top">WLAN</div>
        <div class="inner"></div>
        </div> <!-- wifi列表 -->
    </div>
    <div class="tail">
        ip:192.168.0.1 From JAHSEHKYLE KUIBOYANG
    </div>
</body>
<script>
    async function get_wifi_data() {
        try {
            document.getElementById('loading-ring-overlay').style.display = 'flex';
            console.log("开始获取WiFi数据");
            const response = await fetch('/get_wifi_data');
            document.getElementById('loading-ring-overlay').style.display = 'none';
            if (!response.ok) {
                throw new Error(`HTTP error! Status: ${response.status}`);
            }
            console.log("获取WiFi数据成功");
            var data = await response.json();
            set_wifi_list(data);
            console.log(data);
            
        } catch (error) {
            document.getElementById('loading-ring-overlay').style.display = 'none';
            console.error('获取WiFi数据时出错:', error);
            var data = {};
            set_wifi_list(data);
        }
    }


    const wifi_list = document.querySelector('.inner');
    const item_input = document.getElementById('password-input');
    const submit_button = document.getElementById('submit-button');
    const modal = document.getElementById('modal');

    item_input.addEventListener('input',() => {  // 密码输入框事件
        if (item_input.value.trim() !== ''){
            submit_button.textContent = '确定';
            submit_button.style.backgroundColor = 'white';
            submit_button.style.color = 'black';
        } else {
            submit_button.textContent = '取消';
            submit_button.style.backgroundColor = 'black';
            submit_button.style.color = 'white';
        }
    });

    submit_button.addEventListener('click',() => {  // 确定按钮事件
        const botton_text = submit_button.textContent;
        const password = item_input.value.trim();
        const link_wifi_name = document.getElementById('item-content').textContent.split(':')[1].trim();
        if (botton_text === '确定'){
            alert("连接的wifi名称：" + link_wifi_name + "\n密码：" + password);
        } else {
            hide_modal();
        }
    });

    function set_wifi_list(wifi_json_data) {
        if (Object.keys(wifi_json_data).length == 0){  // 无可用网络
            const list_item_error = document.createElement('div');
            list_item_error.className = 'list_item_error';
            list_item_error.textContent = '未找到任何可用网络，请刷新后重试！';
            wifi_list.appendChild(list_item_error);
        } else {
            for (let i = 0; i < wifi_json_data.wifi_names.length; i++) {  // 循环创建wifi列表项
                const list_item = document.createElement('div');
                list_item.style.display = 'flex';
                list_item.style.flexDirection = 'column';
                list_item.className = 'list_item';
                list_item.addEventListener('click',() => {
                    show_modal(wifi_json_data.wifi_names[i]);
                });
                const list_item_top = document.createElement('div');  // wifi名称和强度
                list_item_top.style.display = 'flex';
                list_item_top.style.justifyContent = 'center';
                list_item_top.style.flexDirection = 'row';

                const list_item_top_wifi_name = document.createElement('p');  // wifi名称
                list_item_top_wifi_name.textContent = `${wifi_json_data.wifi_names[i]}`;
                list_item_top_wifi_name.style.fontSize = '4.5rem';
                list_item_top_wifi_name.style.width = '50rem';
                list_item_top_wifi_name.style.height = '5rem';
                list_item_top_wifi_name.style.marginRight = '1rem';
                // list_item_top_wifi_name.style.backgroundColor = 'white';
                list_item_top_wifi_name.style.display = 'flex';
                list_item_top_wifi_name.style.alignItems = 'center';

                const list_item_top_wifi_strength = document.createElement('p');  // wifi强度
                list_item_top_wifi_strength.textContent = `${wifi_json_data.wifi_rssi[i]} dBm`;
                list_item_top_wifi_strength.style.fontSize = '2rem';
                list_item_top_wifi_strength.style.width = '9rem';
                //list_item_top_wifi_strength.style.backgroundColor = 'white';
                list_item_top_wifi_strength.style.display = 'flex';
                list_item_top_wifi_strength.style.alignItems = 'center';
                list_item_top.appendChild(list_item_top_wifi_name);
                list_item_top.appendChild(list_item_top_wifi_strength);
                list_item.appendChild(list_item_top);

                const list_item_data = document.createElement('div');  // wifi数据
                list_item_data.style.display = 'flex';
                list_item_data.style.justifyContent = 'center';
                list_item_data.style.marginTop = '1rem';
                list_item.appendChild(list_item_data);

                const list_item_data_encryptedTYPE = document.createElement('p');  // 加密类型
                list_item_data_encryptedTYPE.style.fontSize = '1.5rem';
                list_item_data_encryptedTYPE.textContent = `加密类型: ${wifi_json_data.wifi_encryptedTYPE[i]}`;
                list_item_data_encryptedTYPE.style.width = '24rem';
                list_item_data_encryptedTYPE.style.height = '5rem';
                list_item_data_encryptedTYPE.style.marginRight = '1rem';
                // list_item_data_encryptedTYPE.style.backgroundColor = 'white';
                list_item_data_encryptedTYPE.style.display = 'flex';
                list_item_data_encryptedTYPE.style.alignItems = 'center';
                list_item_data.appendChild(list_item_data_encryptedTYPE);

                const list_item_data_channel = document.createElement('p');  // 信道
                list_item_data_channel.style.fontSize = '1.5rem';
                list_item_data_channel.textContent = `信道: ${wifi_json_data.wifi_channel[i]}`;
                list_item_data_channel.style.width = '8rem';
                list_item_data_channel.style.height = '5rem';
                list_item_data_channel.style.marginRight = '1rem';
                // list_item_data_channel.style.backgroundColor = 'white';
                list_item_data_channel.style.display = 'flex';
                list_item_data_channel.style.alignItems = 'center';
                list_item_data.appendChild(list_item_data_channel);

                const list_item_data_MAC = document.createElement('p');  // MAC地址
                list_item_data_MAC.style.fontSize = '1.5rem';
                list_item_data_MAC.textContent = `MAC地址: ${wifi_json_data.wifi_MAC[i]}`;
                list_item_data_MAC.style.width = '25rem';
                list_item_data_MAC.style.height = '5rem';
                list_item_data_MAC.style.marginRight = '1rem';
                // list_item_data_MAC.style.backgroundColor = 'white';
                list_item_data_MAC.style.display = 'flex';
                list_item_data_MAC.style.alignItems = 'center';
                list_item_data_MAC.style.justifyContent = 'flex-end';
                list_item_data.appendChild(list_item_data_MAC);
                wifi_list.appendChild(list_item);
            }
            const tips = document.createElement('p');
            tips.style.fontSize = '1.5rem';
            tips.style.marginTop = '1rem';
            tips.style.textAlign = 'center';
            tips.textContent = '未找到可用网络时，请刷新网页更新列表！';
            wifi_list.appendChild(tips);
        }
    }

    function show_modal(ssid) {
        document.getElementById('item-content').textContent = `SSID: ${ssid}`;
        modal.style.display = 'flex';  // 显示弹窗框
        document.getElementById('modal-overlay').style.display = 'block';  // 显示遮罩层
    }

    function hide_modal() {
        modal.style.display = 'none';  // 隐藏弹窗框
        document.getElementById('modal-overlay').style.display = 'none';  // 隐藏遮罩层
    }


    function resetHTMLFontSize() {
        if (screen.width <= screen.height) {
            document.documentElement.style.fontSize = screen.width / 100 + 'px';
        } else {
            document.documentElement.style.fontSize = screen.height / 200 + 'px';
        }
        
    }

    function LogoRotation() {
        rotationAngle = Math.atan(screen.height / screen.width) * 57.3;
        const logo = document.querySelector('.bg');
        logo.style.transform = `rotate(${rotationAngle}deg)`;
    }

    window.onresize = function() {
        resetHTMLFontSize();
        LogoRotation();
    }

    LogoRotation();
    resetHTMLFontSize();

    get_wifi_data();

    function loop() {
        get_wifi_data();
    }

    // setInterval(loop, 10000);  // 定时刷新
</script>
</html>
)rawliteral";




















String configHtml = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ConfigurationPage</title>
</head>
<style>
    * {
        margin: 0;
        padding: 0;
        color: rgb(82, 82, 82);
        font-family: 'Microsoft YaHei', Arial, sans-serif;
        font-size: 3rem;
    }

    body {
        position: fixed;
        margin: 0;
        padding: 0;
        display: flex;
        flex-direction: column;
        justify-content: center;
        align-items: center;
        height: 100vh;
        width: 100vw;
        opacity: 0;
        transition: opacity 5s ease;
    }    

    .main {
        display: flex;
        flex-direction: column;    /* 垂直排列 */
        justify-content: center;  /* 居中 */
        align-items: center;     /* 居中 */
        height: 100rem;
        width: 80rem;
    }

    .top {
        position: relative;
        width: 100%;
        height: 15rem;
        display: flex;
        justify-content: center;
        align-items: center;
        flex-direction: row;
        background-color: #eeeeee;
        border-radius: 20px;
        box-shadow: 5px 5px 10px rgba(0, 0, 0, 0.5);
    }

    .topinner {
        position: relative;
        color: rgb(82, 82, 82);
        font-size: 3rem;
        user-select: none;
        display: flex;
        justify-content: center;
        align-items: center;
        width: 25%;
        height: 100%;
        transition: color 1s ease;
        z-index: 0;
    }

    .select {
        display: flex;
        justify-content: center;
        align-items: center;
        user-select: none;
        position: absolute;
        width: 20rem;
        height: 9rem;
        transition: left 1s ease;
        z-index: 0;
        /*border: 1px solid rgb(216, 216, 216);*/
    }

    .select-box {
        width: 80%;
        height: 100%;
        background-color: #00adb5;
        border-radius: 5rem;
        z-index: 0;
    }

    .inner {
        user-select: none;
        display: none;
        justify-content: center;
        align-items: center;
        flex-direction: column;
        height: 85rem;
        width: 80rem;
        z-index: 2;
        opacity: 0;
        transition: opacity 3s ease;
    }

    .inner-row {
        margin: 2rem 0;
        display: flex;
        justify-content: center;
        align-items: center;
        flex-direction: row;
        width: 100%;
    }

    .inner-tier {
        margin: 0 2rem;
        display: flex;
        justify-content: center;
        align-items: center;
        flex-direction: column;
        width: 100%;
        height: 32rem;
        border-radius: 3rem;
        box-shadow: 5px 5px 10px rgba(0, 0, 0, 0.5);
    }

    .data-level-1 {
        font-size: 5rem;
        text-align: center;
        width: 14rem;
    }

    .data-level-2 {
        font-size: 10rem;
        text-align: center;
    }

    .data-level-3 {
        font-size: 10rem;
        text-align: center;
    }

    .inner-bg {
        margin-top: 1rem;  /* 距离顶部的距离 */
        top: 0;
        left: 0;
        width: 100%;
        height: 100%;
        z-index: 1;
        display: flex;
        justify-content: center;
        align-items: center;
        border-radius: 20px;
        background-color: #eeeeee;
        box-shadow: 5px 5px 10px rgba(0, 0, 0, 0.5);
    }

    .tail {
        bottom: 0;  
        width: 100%;
        height: 10rem;
        display: flex;
        justify-content: center;
        align-items: center;
        font-size: 2rem;
        color: rgb(82, 82, 82);
    }

    #settings-data {
        margin-top: 1rem;
        font-size: 2rem;
        text-align: center;
    }

    #settings-container {
        display: flex;
        flex-direction: column;
        justify-content: center;
        align-items: center;
        height: 100%;
        width: 80rem;
        border-radius: 20px 20px 0 0;
        border-bottom: 2px dotted rgb(216, 216, 216);
    }

    #saveButton {
        height: 8rem;
        width: 15rem;
        margin: 2rem 3rem 2rem 80%;
        border-radius: 4rem;
        border: none;
        background-color: #00adb5;
        color: white;
        font-size: 3rem;
        cursor: pointer;    /* 鼠标移到按钮上时，鼠标变成手型 */
    }
    #monitor-container {
        display: flex;
        flex-direction: column;
        align-items: center;
        height: 100%;
        width: 100%;
        border-radius: 20px;
    }

    #datetime {
        color: rgb(82, 82, 82);
        margin: 2rem 1rem 0 50%;
        font-style: italic;
        font-size: 3rem;
        text-align: center;
    }

    #monitor-data {
        margin-top: 1rem;
        font-size: 2rem;
        text-align: center;
        width: 90%;
        height: 87%;
        background-color: rgb(168, 188, 143);
    }

    #about-inner {
        overflow-y: auto;
        margin-top: 1rem;
        font-size: 3rem;
        text-align: center;
        width: 90%;
        height: 87%;
        color: rgb(82, 82, 82);
        background-color: rgb(236, 236, 236);
    }

    .bg {
        pointer-events: none;  /* 禁止点击背景 */
        user-select: none;    /* 禁止选中文本 */
        height: 10%;
        width: 200%;
        display: flex;
        position: absolute;
        justify-items: center;
        align-items: center;
        z-index: 3;
        transform: rotate(0deg);
    }

    .bg-text {
        font-weight: 700;
        font-size: 15rem;
        color: rgba(226, 226, 226, 0.5);
    }
</style>
<body>
    <div class="bg">
        <p class="bg-text">JAHSEHKYLE KUIBOYANG</p> <!-- 水印文字 -->
    </div>
    <div class ="main"> 
        <div class="top">
            <div class="select">
                <div class="select-box"></div>
            </div>
            <p class="topinner">Home</p>
            <p class="topinner">Settings</p>
            <p class="topinner">Monitor</p>
            <p class="topinner">About</p>
        </div>
        <div class="inner-bg">
            <div class="inner"> <!-- Home Page -->
                <div class="inner-row">
                    <div class="inner-tier">
                        <p>TVOC</p>
                        <p class="data-level-3" id="tvoc-data">----</p>
                    </div>
                    <div class="inner-tier">
                        <p>CH2O</p>
                        <p class="data-level-3" id="ch2o-data">----</p>
                    </div>
                </div>
                <div class="inner-row">
                    <div class="inner-tier">
                        <p>PM2.5</p>
                        <p class="data-level-2" id="pm25-data">----</p>
                    </div>
                    <div class="inner-tier">
                        <p>CO2</p>
                        <p class="data-level-2" id="co2-data">----</p>
                    </div>
                </div>
                <div class="inner-row">
                    <img src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAYAAADgdz34AAAACXBIWXMAAAsTAAALEwEAmpwYAAAAwklEQVR4nOWVMQoCMRBF370sBVHZ0k7YI+gRVrDzBuIB7ARBLKy9h+AWYqUIVoKRwF9Ikd0mWTE48CFh4D2YhAT+pYZACRjFrgcxBaUDr3KOKTBK3T4dgalJOoJMaW1E2bcFpu0RmSTOYNYgmIfCewK9gKUjKICHev0QwV6QlQOvslDvECK4CDL2CEbq3UIEJ0FyjyBX7x4i2Aiy9gi2MUbUBd6KlU2AqQM3MT6eouGRs1c4SmXAEXgCV2AHdGLBf7s+02ZxVgejeF4AAAAASUVORK5CYII=" alt="tempera ture--v1">
                    <p class="data-level-1" id="temperature-data">
                        ----
                        </p>
                    <div style="width: 5%;"></div>
                    <img src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAYAAADgdz34AAAACXBIWXMAAAsTAAALEwEAmpwYAAAByklEQVR4nN2VOUsDURDHf2ClKNhrmfgdrC0EjaioVSCFWlloLGwUxavwKLQPIZ1fQNDGIkHwqGI8GhsLxVsbxSsa5cF/4RH2baIIggPD25mdmf/sm2Phv1MDsAjkgDsgD9wCe8ACEP5p4CogBbwBnwH8CiSByu8ErweOFKAApIEpIAa0A1FgUvoP2R0AdeVm7gW/AAaBSAAPAeeyP5R/IKWs4FEF6QfWgRvgXaeR+/Q+aoEkShX0TdfiZT4BPDnu3+jHZRfXdZmahFwAi3JMy6kXeJZuW6DdwG4RSK/sN6WbdwHkZDAlh3UreJt0y1b37Ot5zUroHsi6AO7kEJPDteS45AHgRbolYFjPV1bRje2ZCyAvhw5Hx6zo/UaJzpp1AdwWfYEfz6kOxXqj2wJ6gCYXwJ4ApktkGPHhDflmgMZSXZT5ZvCFovY1w+dLYXVHQUblBO/S4JnAqzpPgyY66TPJkQD2Jn8HqLBmZMQFUKnFZYwurRZ18YlszRI01CzZzJST6iyQgmoyo+7q0OkNnjfptfKtkfwYBOB9SUI18dtD3no4ljwKVANjks1VlUUh7Zas5sT7o41oOXY6EmjhF6lVGT/o/NXgf09fpqvFJkKsmD8AAAAASUVORK5CYII=" alt="humidity">
                    <p class="data-level-1" id="humidity-data">
                        ----
                        </p>
                    <div style="width: 5%;"></div>
                    <p class="data-level-1" id="pressure-data">
                        ----
                        </p>
                </div>
            </div>

            <div class="inner"> <!-- Settings Page -->
                <div id="settings-container">
                    <p id="settings-data">This is Settings Page 这是2rem字体</p>
                    <p id="test">这是3rem字体</p>
                </div>
                
                <button id="saveButton">Save</button>
            </div>

            <div class="inner"> <!-- Monitor Page -->
                <div id="monitor-container">
                    <div id="datetime">00:00:00</div>
                    <div id="monitor-data">
                        <canvas id="myChart" width="400" height="200"></canvas>
                    </div>
                </div>
            </div>

            <div class="inner"> <!-- About Page -->
                <p id="about-inner">只需要用到css的一个overflow:auto的属性就可以实现这效果了。 下面我们看看代码和实现的效果。 这次我做的是在一个div里面嵌套的div里实现的滚动条效果，如果你想让外部的div显示滚动条效果，只需在外部的div的class里面设置：overflow:auto即可。 先要给定要设置出现滚动条div的宽高，内容超出给定的宽高之后，即可出现滚动条效果。 延伸：overflow:scroll.如果你想让滚动条自始至终都存在，而不仅仅是内容超出后才出现，可以用overflow:scroll属性。
                    只需要用到css的一个overflow:auto的属性就可以实现这效果了。 下面我们看看代码和实现的效果。 这次我做的是在一个div里面嵌套的div里实现的滚动条效果，如果你想让外部的div显示滚动条效果，只需在外部的div的class里面设置：overflow:auto即可。 先要给定要设置出现滚动条div的宽高，内容超出给定的宽高之后，即可出现滚动条效果。 延伸：overflow:scroll.如果你想让滚动条自始至终都存在，而不仅仅是内容超出后才出现，可以用overflow:scroll属性。
                    只需要用到css的一个overflow:auto的属性就可以实现这效果了。 下面我们看看代码和实现的效果。 这次我做的是在一个div里面嵌套的div里实现的滚动条效果，如果你想让外部的div显示滚动条效果，只需在外部的div的class里面设置：overflow:auto即可。 先要给定要设置出现滚动条div的宽高，内容超出给定的宽高之后，即可出现滚动条效果。 延伸：overflow:scroll.如果你想让滚动条自始至终都存在，而不仅仅是内容超出后才出现，可以用overflow:scroll属性。
                </p>
            </div>
        </div>
    </div>
    <div class="tail">
        ip:192.168.0.1 From JAHSEHKYLE KUIBOYANG
    </div>
</body>
<script>
    const items = document.querySelectorAll('.topinner');
    const indicator = document.querySelector('.select');
    const inners = document.querySelectorAll('.inner');
    let activeIndex = 0;
    let rotationAngle = 0;
    
    setTimeout(() => {
        document.querySelector('body').style.opacity = 1;
    }, 100);

    function updateIndicator(index) {
        const activeItem = items[index];
        indicator.style.left = `${activeItem.offsetLeft}px`;
    }

    items.forEach((item, index) => {   // 给每个导航栏选项添加点击事件
        item.addEventListener('click', () => {    // 点击选项时，更新选项的样式，并更新activeIndex
            activeIndex = index;    // 更新activeIndex位点击的选项的索引
            updateIndicator(activeIndex);  // 更新指示器样式
            inners.forEach((inner) => {  // 隐藏所有选项的页面
                inner.style.display = 'none';
                inner.style.opacity = 0;
            });
            items.forEach((item) => {
                item.style.color = 'rgb(82, 82, 82)';
            });
            item.style.color = 'rgb(255, 255, 255)';
            inners[index].style.display = 'flex';  // 显示对应选项的页面
            setTimeout(() => {   // 延迟显示，防止页面闪烁, 并设置对应选项的页面的透明度为1
                inners[index].style.opacity = 1;  // 显示对应选项的页面
            }, 0);
            
        });
    });

    items[activeIndex].style.color = 'rgb(255, 255, 255)';  // 选中第一个选项的样式
    inners[activeIndex].style.display = 'flex';  // 默认显示第一个选项的页面
    inners[activeIndex].style.opacity = 1;  // 默认显示第一个选项的页面
    updateIndicator(activeIndex); // 默认选中第一个选项

    function LogoRotation() {
        rotationAngle = Math.atan(screen.height / screen.width) * 57.3;
        console.log(rotationAngle);
        const logo = document.querySelector('.bg');
        logo.style.transform = `rotate(${rotationAngle}deg)`;
    }

    function resetHTMLFontSize() {
        document.documentElement.style.fontSize = screen.width / 100 + 'px';
    }

    window.onresize = function() {
        resetHTMLFontSize();
        LogoRotation();
    }

    LogoRotation();
    resetHTMLFontSize();

    async function GetSensorData() {
        try {
            const response = await fetch('/GetSensorData');
            const data = await response.json();
            document.getElementById('tvoc-data').innerHTML = data.tvoc;
            document.getElementById('ch2o-data').innerHTML = data.ch2o;
            document.getElementById('pm25-data').innerHTML = data.pm25;
            document.getElementById('co2-data').innerHTML = data.co2;
            document.getElementById('temperature-data').innerHTML = data.temperature + '<font style="font-size: 3rem">℃</font>';
            document.getElementById('humidity-data').innerHTML = data.humidity + '<font style="font-size: 3rem">%</font>';
            document.getElementById('pressure-data').innerHTML = data.pressure + '<font style="font-size: 3rem">hPa</font>';
        } catch (error) {
            console.error('Error fetching data:', error);
        }
    }

    function loopFunction() {
        GetSensorData();
        document.getElementById('datetime').innerHTML=new Date().toLocaleString();
    }

    setInterval(loopFunction, 1000);
</script>
</html>
)rawliteral";
}


#endif // CONFIGHTML_H
