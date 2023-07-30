/*	==============================================================================
 *	Student Name: Wai Chan
 *	SID: 2053 8079
 *	Subject: PPA
 *	Assignment: PPA Project 
 *	© 2023 Wai Chan. All Rights Reserved
============================================================================== */

var colorPicker = new iro.ColorPicker("#picker", {
    // Set the size of the color picker
    width: 350,
    handleRadius: 1,
    padding: 0,
    margin: 20,
    sliderSize: 15
  });

getRGBVal = async function() {
    let rgbVal =  document.getElementById("RGBVal").innerHTML;
    rgbVal = rgbVal.substring(4).slice(0, -1).replace(/\s/g, '');

    // let colorVal = "http://localhost/ppa?RGB=" + rgbVal;
    let colorVal = "https://bu3capc3vk.execute-api.us-east-1.amazonaws.com/ppa?rgb=" + rgbVal;
    // console.log(colorVal);
    new Promise(function(resolve) {
        let req = new XMLHttpRequest();
        req.open('POST', colorVal);
        req.send();
    });
}

colorPicker.on(['color:init', 'color:change'], function(color) {
    // console.log(color.rgbString);
    document.getElementById("RGBVal").innerHTML = color.rgbString;
    getRGBVal();
});
