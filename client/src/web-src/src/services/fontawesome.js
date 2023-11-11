/* import the fontawesome core */
import { library } from "@fortawesome/fontawesome-svg-core";

/* import specific icons */
import { faPlus, faRightToBracket, faArrowLeft, faSpinner } from "@fortawesome/free-solid-svg-icons";

/* add icons to the library */

const fontawesome = {
	install() {
		library.add(faPlus, faRightToBracket, faArrowLeft, faSpinner);
	},
};

export default fontawesome;
