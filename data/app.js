"use strict";

class App {

    navigationContainer = null;
    contentContainer = null;

    PrepareDOM() {

        let self = this;

        self.navigationContainer = $("<div id='navigationContainer'></div>");
        self.contentContainer = $("<div id='contentContainer'></div>");
        self.navigationContainer.appendTo("#appContainer");
        self.contentContainer.appendTo("#appContainer");

        self.navigationContainer.append("<div class='navButton' id='navVariables'>Variables</div>");

        self.navVariables = $("#navVariables");
        self.navVariables.on("click", function () {

            let container = $(self.contentContainer);

            // Détruire proprement l'ancien widget
            if (container.data("custom-VariableNavigator")) {
                container.VariableNavigator("destroy");
            }

            container.empty();

            // Recréation
            container.VariableNavigator({});
        });

    }


    /*

    DI = [];
    AV = [];
    getData_busy = false;
    configDI = {};
    configAV = {};

    constructor(config) {

        this.configDI = config.DI ?? {};
        this.configAV = config.AV ?? {};
        this.PrepareObjects();
    }
	
    AddHelps() {

        // Get all objects that are help button
        [].forEach.call(document.querySelectorAll(".buttonHelpPopup"), function (el) {
        	
            el.addEventListener("click", () => {
            	
                // Remove all dialogs
                [].forEach.call(document.querySelectorAll("dialog"), function (el) {
                    el.remove();
                });
            	
                let title = document.createElement('p');
                title.classList.add("title")
                title.innerHTML = el.getAttribute("title");
            	
                let p = document.createElement('p');
                p.innerHTML = el.getAttribute("text");
                let dialog = document.createElement('dialog');
                dialog.appendChild(title);
                dialog.appendChild(p);
                dialog.setAttribute("closedby", "any");

                document.body.append(dialog);
                dialog.showModal();
            });
        });
    }

    ReplaceInfos() {
        try {
            var httpRequest = new XMLHttpRequest();
            httpRequest.onreadystatechange = function () {

                if (httpRequest.readyState == 4 && httpRequest.status == 200) {

                    var jsonObject = JSON.parse(httpRequest.responseText);
                    document.getElementById("DevEUI").innerHTML = jsonObject.DevEUI;
                    document.getElementById("AppKEY").innerHTML = jsonObject.AppKEY;
                    document.getElementById("AppEUI").innerHTML = jsonObject.AppEUI;

                    document.getElementById("mDNS").value = jsonObject.mDNS;
                    document.getElementById("WSTA_SSID").value = jsonObject.WSTA_SSID;
                    document.getElementById("WSTA_pwd").value = jsonObject.WSTA_pwd;
                    document.getElementById("WAP_ipAddress").value = jsonObject.WAP_ipAddress;
                    document.getElementById("WAP_ipGateway").value = jsonObject.WAP_ipGateway;
                    document.getElementById("WAP_ipMask").value = jsonObject.WAP_ipMask;

                    document.getElementById("sendPeriod").value = jsonObject.sendPeriod;
                }
            };
            httpRequest.open("GET", "/INFOS");
            httpRequest.send();
        } catch (e) {
            console.error("Cannot update informations");
        }
    }

    PrepareObjects() {

        let self = this;

        // DI parts
        if (self.configDI != null && self.configDI.number != null) {

            for (let i = 0; i < self.configDI.number; i++) {

                self.DI[i] = {};
                self.DI[i].value = false;
                self.DI[i].changes = 0;
                self.DI[i].memValue = false;
            }
        }

        // AV parts
        if (self.configAV != null && self.configAV.number != null) {

            for (let i = 0; i < self.configAV.number; i++) {

                self.AV[i] = {};
                self.AV[i].value = false;
                self.AV[i].changes = 0;
                self.AV[i].memValue = false;
            }
        }
    }

    PrepareDOM() {

        let self = this;

        let table = document.getElementById("data");
        [].forEach.call(self.configDI.datarefs, function (e, i) {

            let newRow = table.insertRow(-1);
            let newCell0 = newRow.insertCell(0);
            newCell0.className = 'col2';
            newCell0.innerHTML = `${e.name}`;
            let newCell1 = newRow.insertCell(1);
            newCell1.className = 'col3';
            newCell1.innerHTML = `<div class='DICont' id='${e.jsonProperty}'><div class='DI' value='unknown'></div><div class='changes'></div></div>`;
        });

        [].forEach.call(self.configAV.datarefs, function (e, i) {

            let newRow = table.insertRow(-1);
            let newCell0 = newRow.insertCell(0);
            newCell0.className = 'col2';
            newCell0.innerHTML = `${e.name}`;
            let newCell1 = newRow.insertCell(1);
            newCell1.className = 'col3';
            newCell1.innerHTML = `<div class='AVCont' id='${e.jsonProperty}'><div class='AV'>-</div></div>`;
        });
    }

    FormatString(rawValue, numberOfDecimal, unit) {
        return `${Number.parseFloat(rawValue).toFixed(numberOfDecimal)} ${unit}`;
    }

    async SendIPAddresses() {

        let mDNS = document.getElementById("mDNS").value;
        let WSTA_SSID = document.getElementById("WSTA_SSID").value;
        let WSTA_pwd = document.getElementById("WSTA_pwd").value;
        let WAP_ipAddress = document.getElementById("WAP_ipAddress").value;
        let WAP_ipGateway = document.getElementById("WAP_ipGateway").value;
        let WAP_ipMask = document.getElementById("WAP_ipMask").value;

        const url = "/IP";
        const request1 = new Request(url, {

            method: "POST",
            body: JSON.stringify({
                mDNS: mDNS,
                WSTA_SSID: WSTA_SSID,
                WSTA_pwd: WSTA_pwd,
                WAP_ipAddress: WAP_ipAddress,
                WAP_ipGateway: WAP_ipGateway,
                WAP_ipMask: WAP_ipMask
            }),
            headers: {

                'Content-Type': 'application/json'
            },
            mode: 'cors'
        });

        try {

            const response = await fetch(request1);
            if (!response.ok) {

                throw new Error(`Response status: ${response.status}`);
            }

            if (confirm("Do you want to restart device?")) {
                document.getElementById("formReset").submit();
            }
        } catch (error) {}
    }

    async SendSettings() {

        let sendPeriod = document.getElementById("sendPeriod").value;

        const url = "/SETTINGS";
        const request1 = new Request(url, {

            method: "POST",
            body: JSON.stringify({
                sendPeriod: sendPeriod
            }),
            headers: {

                'Content-Type': 'application/json'
            },
            mode: 'cors'
        });

        try {

            const response = await fetch(request1);
            if (!response.ok) {

                throw new Error(`Response status: ${response.status}`);
            }

        } catch (error) {}
    }

    async getData() {

        let self = this;

        if (!self.getData_busy) {

            self.getData_busy = true;

            const url = "/DATAS";
            const request1 = new Request(url, {

                method: "GET",
                headers: {

                    'Content-Type': 'application/json'
                },
                mode: 'cors'
            });

            try {

                const response = await fetch(request1);
                if (!response.ok) {

                    throw new Error(`Response status: ${response.status}`);
                }

                // Remove all error flags of DI DOM elements
                [].forEach.call(document.querySelector("#data").querySelectorAll(":scope .DI"), function (el) {
                    el.removeAttribute('error');
                });

                // Remove all error flags of AV DOM elements
                [].forEach.call(document.querySelector("#data").querySelectorAll(":scope .AV"), function (el) {
                    el.removeAttribute('error');
                });

                const json = await response.json();

                // Get values from json response
                [].forEach.call(self.DI, function (e, i) {
                    e.value = json[self.configDI.datarefs[i].jsonProperty]
                });

                // Get values from json response
                [].forEach.call(self.AV, function (e, i) {
                    e.value = json[self.configAV.datarefs[i].jsonProperty]
                });

                // Detect changes
                [].forEach.call(self.DI, function (e) {
                    if (e.value != e.memValue) {
                        e.changes++;
                        e.memValue = e.value;
                    }
                });

                // Set visual style and values for DI
                [].forEach.call(self.DI, function (e, i) {
                    document.getElementById(self.configDI.datarefs[i].jsonProperty).firstChild.setAttribute('value', e.value ? "active" : "inactive");
                    document.getElementById(self.configDI.datarefs[i].jsonProperty).querySelector(":scope .changes").innerHTML = ` (${e.changes})`;
                });

                // Set AV values
                [].forEach.call(self.AV, function (e, i) {
                    document.getElementById(self.configAV.datarefs[i].jsonProperty).firstChild.innerHTML = self.FormatString(e.value, self.configAV.datarefs[i].decimal, self.configAV.datarefs[i].unit);
                });
            } catch (error) {

                // Add error flags to all DI DOM elements
                [].forEach.call(document.querySelector("#data").querySelectorAll(":scope .DI"), function (el) {
                    el.setAttribute('error', 'error');
                });

                // Add error flags to all AV DOM elements
                [].forEach.call(document.querySelector("#data").querySelectorAll(":scope .AV"), function (el) {
                    el.setAttribute('error', 'error');
                });
            } finally {

                self.getData_busy = false;
            }
        }
    }*/
}