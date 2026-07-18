declare var DevExpress: any;
declare var $: any;

$(function () {

    function isDev(): boolean {
        return window.location.protocol === "file:";
    }

    // the widget definition, where "custom" is the namespace,
    // "MadMaxSystem" the widget name
    $.widget("custom.MadMaxSystem", {

        container: null,
        madmaxSystemContainer: null,
        madmaxSystemInterfacesEth: null,
        getData_busy: false,
        interval: null,

        options: {},

        // The constructor, set DOM
        _create: function () {

            let self = this;

            self.container = self.element;

            self._uiCreateComposant();

            return self;
        },

        // Refresh composant (js -> css styling, resize (less possible),…)
        _refresh: function () {

        },

        // Events bound via _on are removed automatically
        // revert other modifications here, remove DOM
        _destroy: function () {

            let self = this;

            self.interval && clearInterval(self.interval);
        },

        // _setOptions is called with a hash of all options that are changing
        // always refresh when changing options
        _setOptions: function () {

            // _super and _superApply handle keeping the right this-context
            this._superApply(arguments);
            this._refresh();
        },

        // _setOption is called for each individual option that is changing
        _setOption: function (key: any, value: any) {

            // we can disable change in return before _super

            this._super(key, value);
        },

        _uiCreateComposant: function () {

            let self = this;

            self.container.html('');

            self.madmaxSystemContainer = $("<div id='MadMaxSystemContainer'></div>").appendTo(self.container);
            self.madmaxInterfacesContainer = $("<div id='MadMaxInterfacesContainer'></div>").appendTo(self.madmaxSystemContainer);

            let form1 = $("<form></form>").appendTo(self.madmaxInterfacesContainer);

            self.madmaxSystemInterfacesEth = $("<div class='madmaxSystemInterface' id='madmaxSystemInterfacesEth'></div>").appendTo(form1);
            self.madmaxSystemInterfacesEth.append(`<div class="title">Propriété de l'interface ETH</div>`);
            self.madmaxSystemInterfacesEth.append(`<div class="property name"><span>Désignation :</span><input disabled='disabled' type="text" id="eth-name" value='ETH' /></div>`);
            self.madmaxSystemInterfacesEth.append(`<div class="property dhcp"><span>DHCP :</span><input id='eth-dhcp' disabled='disabled' type='checkbox' value='true' /></div>`);
            self.madmaxSystemInterfacesEth.append(`<div class="property ip"><span>Adresse IP :</span><input id='eth-ip' disabled='disabled' type='text' placeholder='xxx.xxx.xxx.xxx' /></div>`);
            self.madmaxSystemInterfacesEth.append(`<div class="property netmask"><span>Masque de sous-réseau :</span><input id='eth-netmask' disabled='disabled' type='text' placeholder='xxx.xxx.xxx.xxx' /></div>`);
            self.madmaxSystemInterfacesEth.append(`<div class="property gateway"><span>Passerelle :</span><input id='eth-gateway' disabled='disabled' type='text' placeholder='xxx.xxx.xxx.xxx' /></div>`);
            self.madmaxSystemInterfacesEth.append(`<div class="property dns1"><span>DNS 1 :</span><input id='eth-dns1' disabled='disabled' type='text' placeholder='xxx.xxx.xxx.xxx' /></div>`);
            self.madmaxSystemInterfacesEth.append(`<div class="property dns2"><span>DNS 2 :</span><input id='eth-dns2' disabled='disabled' type='text' placeholder='xxx.xxx.xxx.xxx' /></div>`);

            // Add submit button
            let buttons = $("<div class='buttons'>").appendTo(self.madmaxSystemInterfacesEth);
            let submitButton = $("<button type='submit'>Mettre à jour</button></div>").appendTo(buttons);

            // Handle form submission
            form1.on("submit", (e: any) => {

                e.preventDefault();

                submitButton.attr("disabled", "disabled");

                let newValue = {

                    dhcp: $("#eth-dhcp", self.madmaxSystemContainer).is(":checked"),
                    ip: $("#eth-ip", self.madmaxSystemContainer).val(),
                    netmask: $("#eth-netmask", self.madmaxSystemContainer).val(),
                    gateway: $("#eth-gateway", self.madmaxSystemContainer).val(),
                    dns1: $("#eth-dns1", self.madmaxSystemContainer).val(),
                    dns2: $("#eth-dns2", self.madmaxSystemContainer).val()
                };

                console.log("Updating ETH Interface with new value:", JSON.stringify(newValue));

                self.setInterfaceProperties("ETH", newValue)
                    .then(() => {
                        //$(".property #accum-value", self.madmaxSystemContainer).html(newDisplayedValue).addClass("stale"); // Update displayed value and add stale class as it may take a moment for the new value to be reflected in the details view
                        submitButton.removeAttr("disabled");
                    })
                    .catch((error: string) => {
                        console.error("Error updating ETH Interface:", error);
                        submitButton.removeAttr("disabled");
                    });

                return false;
            });

            let form2 = $("<form></form>").appendTo(self.madmaxInterfacesContainer);
            
            self.madmaxSystemInterfacesSta = $("<div class='madmaxSystemInterface' id='madmaxSystemInterfacesSta'></div>").appendTo(form2);
            self.madmaxSystemInterfacesSta.append(`<div class="title">Propriété de l'interface Station WiFi</div>`);
            self.madmaxSystemInterfacesSta.append(`<div class="property name"><span>Désignation :</span><input disabled='disabled' type="text" id="sta-name" value='STA' /></div>`);
            self.madmaxSystemInterfacesSta.append(`<div class="property dhcp"><span>DHCP :</span><input id='sta-dhcp' disabled='disabled' type='checkbox' value='true' /></div>`);
            self.madmaxSystemInterfacesSta.append(`<div class="property ip"><span>Adresse IP :</span><input id='sta-ip' disabled='disabled' type='text' placeholder='xxx.xxx.xxx.xxx' /></div>`);
            self.madmaxSystemInterfacesSta.append(`<div class="property netmask"><span>Masque de sous-réseau :</span><input id='sta-netmask' disabled='disabled' type='text' placeholder='xxx.xxx.xxx.xxx' /></div>`);
            self.madmaxSystemInterfacesSta.append(`<div class="property gateway"><span>Passerelle :</span><input id='sta-gateway' disabled='disabled' type='text' placeholder='xxx.xxx.xxx.xxx' /></div>`);
            self.madmaxSystemInterfacesSta.append(`<div class="property dns1"><span>DNS 1 :</span><input id='sta-dns1' disabled='disabled' type='text' placeholder='xxx.xxx.xxx.xxx' /></div>`);
            self.madmaxSystemInterfacesSta.append(`<div class="property dns2"><span>DNS 2 :</span><input id='sta-dns2' disabled='disabled' type='text' placeholder='xxx.xxx.xxx.xxx' /></div>`);
            self.madmaxSystemInterfacesSta.append(`<div class="property ssid"><span>SSID :</span><input id='sta-ssid' disabled='disabled' type='text' placeholder='SSID' /></div>`);
            self.madmaxSystemInterfacesSta.append(`<div class="property password"><span>Password :</span><input id='sta-password' disabled='disabled' type='password' placeholder='********' /></div>`);

            // Add submit button
            buttons = $("<div class='buttons'>").appendTo(self.madmaxSystemInterfacesSta);
            submitButton = $("<button type='submit'>Mettre à jour</button></div>").appendTo(buttons);

            // Handle form submission
            form2.on("submit", (e: any) => {

                e.preventDefault();

                submitButton.attr("disabled", "disabled");

                let newValue = {

                    dhcp: $("#sta-dhcp", self.madmaxSystemContainer).is(":checked"),
                    ip: $("#sta-ip", self.madmaxSystemContainer).val(),
                    netmask: $("#sta-netmask", self.madmaxSystemContainer).val(),
                    gateway: $("#sta-gateway", self.madmaxSystemContainer).val(),
                    dns1: $("#sta-dns1", self.madmaxSystemContainer).val(),
                    dns2: $("#sta-dns2", self.madmaxSystemContainer).val(),
                    ssid: $("#sta-ssid", self.madmaxSystemContainer).val(),
                    password: $("#sta-password", self.madmaxSystemContainer).val()
                };

                console.log("Updating STA Interface with new value:", JSON.stringify(newValue));

                self.setInterfaceProperties("STA", newValue)
                    .then(() => {
                        //$(".property #accum-value", self.madmaxSystemContainer).html(newDisplayedValue).addClass("stale"); // Update displayed value and add stale class as it may take a moment for the new value to be reflected in the details view
                        submitButton.removeAttr("disabled");
                    })
                    .catch((error: string) => {
                        console.error("Error updating STA Interface:", error);
                        submitButton.removeAttr("disabled");
                    });

                return false;
            });

            let form3 = $("<form></form>").appendTo(self.madmaxInterfacesContainer);

            self.madmaxSystemInterfacesWap = $("<div class='madmaxSystemInterface' id='madmaxSystemInterfacesWap'></div>").appendTo(form3);
            self.madmaxSystemInterfacesWap.append(`<div class="title">Propriété de l'interface WiFi Access Point</div>`);
            self.madmaxSystemInterfacesWap.append(`<div class="property name"><span>Désignation :</span><input disabled='disabled' type="text" id="wap-name" value='WAP' /></div>`);
            self.madmaxSystemInterfacesWap.append(`<div class="property ip"><span>Adresse IP :</span><input id='wap-ip' disabled='disabled' type='text' placeholder='xxx.xxx.xxx.xxx' /></div>`);
            self.madmaxSystemInterfacesWap.append(`<div class="property netmask"><span>Masque de sous-réseau :</span><input id='wap-netmask' disabled='disabled' type='text' placeholder='xxx.xxx.xxx.xxx' /></div>`);
            self.madmaxSystemInterfacesWap.append(`<div class="property gateway"><span>Passerelle :</span><input id='wap-gateway' disabled='disabled' type='text' placeholder='xxx.xxx.xxx.xxx' /></div>`);
            self.madmaxSystemInterfacesWap.append(`<div class="property ssid"><span>SSID :</span><input id='wap-ssid' disabled='disabled' type='text' placeholder='SSID' /></div>`);
            self.madmaxSystemInterfacesWap.append(`<div class="property password"><span>Password :</span><input id='wap-password' disabled='disabled' type='password' placeholder='********' /></div>`);

            // Add submit button
            buttons = $("<div class='buttons'>").appendTo(self.madmaxSystemInterfacesWap);
            submitButton = $("<button type='submit'>Mettre à jour</button></div>").appendTo(buttons);

            // Handle form submission
            form3.on("submit", (e: any) => {

                e.preventDefault();

                submitButton.attr("disabled", "disabled");

                let newValue = {

                    ip: $("#wap-ip", self.madmaxSystemContainer).val(),
                    netmask: $("#wap-netmask", self.madmaxSystemContainer).val(),
                    gateway: $("#wap-gateway", self.madmaxSystemContainer).val(),
                    ssid: $("#wap-ssid", self.madmaxSystemContainer).val(),
                    password: $("#wap-password", self.madmaxSystemContainer).val()
                };

                console.log("Updating WAP Interface with new value:", JSON.stringify(newValue));

                self.setInterfaceProperties("WAP", newValue)
                    .then(() => {
                        //$(".property #accum-value", self.madmaxSystemContainer).html(newDisplayedValue).addClass("stale"); // Update displayed value and add stale class as it may take a moment for the new value to be reflected in the details view
                        submitButton.removeAttr("disabled");
                    })
                    .catch((error: string) => {
                        console.error("Error updating WAP Interface:", error);
                        submitButton.removeAttr("disabled");
                    });

                return false;
            });

            self._getData()
                .then((json: any | null) => {

                    if (json === null)
                        return;

                    self.uiDisplayInterfacesProperties(json, self.madmaxSystemContainer);
                })
                .catch((error: any) => {
                    console.error("Erreur getData:", error);
                });

        },

        _getData: async function () {

            let self = this;

            if (this.getData_busy)
                return null;

            this.getData_busy = true;

            try {

                // Simulate API response with mock data in development, otherwise fetch from the server
                if (isDev()) {

                    let mock = [
                        {
                            "name": "ETH",
                            "dhcp": true,
                            "ip": "192.168.10.244",
                            "netmask": "255.255.255.0",
                            "gateway": "192.168.10.252",
                            "dns1": "192.168.10.252",
                            "dns2": "0.0.0.0"
                        },
                        {
                            "name": "STA",
                            "dhcp": true,
                            "ip": "0.0.0.0",
                            "netmask": "0.0.0.0",
                            "gateway": "0.0.0.0",
                            "dns1": "0.0.0.0",
                            "dns2": "0.0.0.0",
                            "ssid": "Galaxy A33 5G7C18",
                            "password": "T41l5l0v3r43v3r"
                        },
                        {
                            "name": "WAP",
                            "ip": "192.168.11.1",
                            "netmask": "255.255.255.0",
                            "gateway": "192.168.11.1",
                            "ssid": "AP_Madmax",
                            "password": "password"
                        }
                    ];

                    // petit délai pour simuler un vrai fetch (optionnel)
                    await new Promise(r => setTimeout(r, 100));

                    return mock;
                } else {

                    const response = await fetch(`/API/Interfaces`);

                    if (!response.ok) {
                        throw new Error(`HTTP ${response.status}`);
                    }

                    const json = await response.json();

                    return json as any;
                }

            } finally {
                this.getData_busy = false;
            }
        },

        uiDisplayInterfacesProperties: function (json: any, container: any) {

            let self = this;

            // ETH
            const eth = json.find((obj: any) => obj.name === "ETH");
            $("#eth-dhcp", container).removeAttr('disabled').prop("checked", eth.dhcp);
            $("#eth-ip", container).removeAttr('disabled').val(eth.ip);
            $("#eth-netmask", container).removeAttr('disabled').val(eth.netmask);
            $("#eth-gateway", container).removeAttr('disabled').val(eth.gateway);
            $("#eth-dns1", container).removeAttr('disabled').val(eth.dns1);
            $("#eth-dns2", container).removeAttr('disabled').val(eth.dns2);
            
            // STA
            const sta = json.find((obj: any) => obj.name === "STA");
            $("#sta-dhcp", container).removeAttr('disabled').prop("checked", sta.dhcp);
            $("#sta-ip", container).removeAttr('disabled').val(sta.ip);
            $("#sta-netmask", container).removeAttr('disabled').val(sta.netmask);
            $("#sta-gateway", container).removeAttr('disabled').val(sta.gateway);
            $("#sta-dns1", container).removeAttr('disabled').val(sta.dns1);
            $("#sta-dns2", container).removeAttr('disabled').val(sta.dns2);
            $("#sta-ssid", container).removeAttr('disabled').val(sta.ssid);
            $("#sta-password", container).removeAttr('disabled').val(sta.password);

            // WAP
            const wap = json.find((obj: any) => obj.name === "WAP");
            $("#wap-ip", container).removeAttr('disabled').val(wap.ip);
            $("#wap-netmask", container).removeAttr('disabled').val(wap.netmask);
            $("#wap-gateway", container).removeAttr('disabled').val(wap.gateway);
            $("#wap-ssid", container).removeAttr('disabled').val(wap.ssid);
            $("#wap-password", container).removeAttr('disabled').val(wap.password);
        },

        setInterfaceProperties: async function (interfaceName: string, value: any) {

            let self = this;

            try {

                const response = await fetch(`/API/Interfaces/${interfaceName.toLowerCase()}`, {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json'
                    },
                    body: JSON.stringify(value)
                });
            } catch (error) {
                console.error(`Error updating ${interfaceName} Interface:`, error);
            }
        }
    });
});
