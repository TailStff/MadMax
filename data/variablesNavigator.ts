declare var DevExpress: any;
declare var $: any;

$(function () {

    type Variable = {
        name: string;
        value: any;
    };

    type Variables = Variable[];

    function isDev(): boolean {
        return window.location.protocol === "file:";
    }

    // the widget definition, where "custom" is the namespace,
    // "VariableNavigator" the widget name
    $.widget("custom.VariableNavigator", {

        container: null,
        manageVariablesContainer: null,
        getData_busy: false,

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

            self.manageVariablesContainer = $("<div id='VariablesContainer'></div>").appendTo(self.container);

            let list = $("<ul></ul>").appendTo(self.manageVariablesContainer);
            let detail = $("<div id='VariableDetail'></div>").appendTo(self.manageVariablesContainer);

            self.getData()
                .then((json: Variables | null) => {

                    if (json === null) {

                        console.warn("getData is busy, please wait.");
                        return;
                    }

                    json.forEach((variable: Variable) => {

                        let item = $("<li></li>").appendTo(list);
                        item.text(variable.name);
                        item.on("click", () => { detail.text(`${variable.name}: ${variable.value}`); });
                    });

                })
                .catch((error: any) => {

                    console.error("Erreur getData:", error);
                });
        },

        /// Get variables list from the server, returns null if a request is already in progress, otherwise returns a promise that resolves to the variables list
        getData: async function (): Promise<Variables | null> {

            if (this.getData_busy)
                return null;

            this.getData_busy = true;

            try {

                // Simulate API response with mock data in development, otherwise fetch from the server
                if (isDev()) {

                    const mock: Variables = [

                        { name: "test_bool", value: false },
                        { name: "test_int", value: 42 },
                        { name: "test_float", value: 3.14 },
                        { name: "test_double", value: 3.1415 }
                    ];

                    // petit délai pour simuler un vrai fetch (optionnel)
                    await new Promise(r => setTimeout(r, 100));

                    return mock;
                } else {

                    const response = await fetch("/API/Variables");

                    if (!response.ok) {
                        throw new Error(`HTTP ${response.status}`);
                    }

                    const json = await response.json();

                    return json as Variables;
                }

            } finally {
                this.getData_busy = false;
            }
        }
    });
});
