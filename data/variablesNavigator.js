"use strict";
$(function () {
    function isDev() {
        return window.location.protocol === "file:";
    }
    $.widget("custom.VariableNavigator", {
        container: null,
        manageVariablesContainer: null,
        getData_busy: false,
        options: {},
        _create: function () {
            let self = this;
            self.container = self.element;
            self._uiCreateComposant();
            return self;
        },
        _refresh: function () {
        },
        _destroy: function () {
        },
        _setOptions: function () {
            this._superApply(arguments);
            this._refresh();
        },
        _setOption: function (key, value) {
            this._super(key, value);
        },
        _uiCreateComposant: function () {
            let self = this;
            self.container.html('');
            self.manageVariablesContainer = $("<div id='VariablesContainer'></div>").appendTo(self.container);
            let list = $("<ul></ul>").appendTo(self.manageVariablesContainer);
            let detail = $("<div id='VariableDetail'></div>").appendTo(self.manageVariablesContainer);
            self.getData()
                .then((json) => {
                if (json === null) {
                    console.warn("getData is busy, please wait.");
                    return;
                }
                json.forEach((variable) => {
                    let item = $("<li></li>").appendTo(list);
                    item.text(variable.name);
                    item.on("click", () => { detail.text(`${variable.name}: ${variable.value}`); });
                });
            })
                .catch((error) => {
                console.error("Erreur getData:", error);
            });
        },
        getData: async function () {
            if (this.getData_busy)
                return null;
            this.getData_busy = true;
            try {
                if (isDev()) {
                    const mock = [
                        { name: "test_bool", value: false },
                        { name: "test_int", value: 42 },
                        { name: "test_float", value: 3.14 },
                        { name: "test_double", value: 3.1415 }
                    ];
                    await new Promise(r => setTimeout(r, 100));
                    return mock;
                }
                else {
                    const response = await fetch("/API/Variables");
                    if (!response.ok) {
                        throw new Error(`HTTP ${response.status}`);
                    }
                    const json = await response.json();
                    return json;
                }
            }
            finally {
                this.getData_busy = false;
            }
        }
    });
});
