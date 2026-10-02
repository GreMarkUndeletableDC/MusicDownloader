// ==UserScript==
// @name         New Userscript
// @namespace    http://tampermonkey.net/
// @version      2024-08-13
// @description  try to take over the world!
// @author       You
// @match        https://music.163.com/
// @icon         https://www.google.com/s2/favicons?sz=64&domain=163.com
// @grant        none
// ==/UserScript==

(function() {
    'use strict';
    var intervalId = window.setInterval(function() {
        if (document.readyState === "complete") {
            clearInterval(intervalId);
            console.log("===========================================");
            var doc = document.getElementById('g_iframe').contentDocument;
            var tab = doc.getElementsByClassName('m-table ')[0];
            var body = tab.getElementsByTagName('tbody')[0];
            var trs = body.childNodes;
            var s = new String();
            trs.forEach(child => {
                var span = child.getElementsByClassName('left')[0].getElementsByClassName('hd')[0].getElementsByClassName('ply')[0];
                s = s + span.getAttribute('data-res-id') + "\n";
                //console.log(span.getAttribute('data-res-id'));
            });
            console.log(s);
        }
    }, 4000);
})();