#ifndef configPage_h
#define configPage_h

const char config_page[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html>
<head>
  <title>Config Wi-Fi</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
  body {
    max-width: 480px;
    margin: 0 auto;
  }
  input[type=text], select {
    width: 100%;
    padding: 12px 20px;
    margin: 8px 0;
    display: inline-block;
    border: 1px solid #ccc;
    border-radius: 4px;
    box-sizing: border-box;
  }
  input[type=submit] {
    width: 100%;
    background-color: #4CAF50;
    color: white;
    padding: 14px 20px;
    margin: 8px 0;
    border: none;
    border-radius: 4px;
    cursor: pointer;
  }
  input[type=submit]:hover {
    background-color: #45a049;
  }

  .container {
    border-radius: 5px;
    background-color: #f2f2f2;
    padding: 20px;
  }
  </style>
 </head>
<body>
<div class="container">
  <form action="/do_config" method="POST">
      <label for="ssid">SSID</label>
      <input type="text" id ="ssid" name="ssid" placeholder="Nama Wi-Fi"><br>
      <label for="usepass">Use password?</label>
      <select id="usepass" name="usepass">
        <option value="n">NO</option>
        <option value="y">YES</option>
      </select>
      <div id="passwordelement" style="display:none;">
      <label for="pass">Password</label>
      <input type="text" id ="pass" name="pass" placeholder="Password Wi-Fi"><br>
      </div>
      <label for="staticip">IP Settings</label>
      <select id="staticip" name="staticip">
        <option value="no">DHCP</option>
        <option value="yes">STATIC</option>
      </select>
      <div id="staticelement" style="display:none;">
      <label for="ip">IP Address</label>
      <input type="text" id ="ip" name="ip" placeholder="Contoh : 192.168.1.200"><br>
      <label for="gateway">Gateway Address</label>
      <input type="text" id ="gateway" name="gateway" placeholder="Contoh : 192.168.1.1"><br>
      </div>
      <input type="submit" value ="Submit">
  </form>
</div>
<script>
const staticip = document.getElementById('staticip')
const staticelement = document.getElementById('staticelement')
staticip.addEventListener("change", () => {
  if (staticip.value == 'yes') {
    staticelement.style.display = "initial"
  } else {
    staticelement.style.display = "none"
  }
})
const usepass = document.getElementById('usepass')
const passwordelement = document.getElementById('passwordelement')
usepass.addEventListener("change", () => {
  if (usepass.value == 'y') {
    passwordelement.style.display = "initial"
  } else {
    passwordelement.style.display = "none"
  }
})
</script>
</body></html>)rawliteral";

#endif
