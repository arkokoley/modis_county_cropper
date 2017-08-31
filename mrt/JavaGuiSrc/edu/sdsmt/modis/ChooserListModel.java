/*********************************************************************************
    NAME:	ChooserListModel
	PURPOSE:
		The purpose of this class is to create a data model based on the concept of
		document/view architecture in an attempt to isolate the data model for List
		box.

	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development
	  1.00   Fall'00  Shujing Jia			         Java  Modifications

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:
        Java is inherently platform indepentent so the compiled byte code can be
    executed on any of platforms (e.g. Windows, Unix, and Linux). Virtually, there
    is no limitation on running Java byte codes.
		However there is compiler requirement regarding JDK package (version 2.0).

	PROJECT:	ModisModel
	NOTES:
		    The data model is designed based on document/view architecture to separate
			the data model from List box view.

*********************************************************************************/

package edu.sdsmt.modis;

import java.util.*;
import javax.swing.*;
import javax.swing.event.*;

   /*********************************************************************************
    NAME:	ChooserListModel
	PURPOSE:
			  To implement a concrete ListModel class from ListModel interface. Most
			  of the function prototypes are defined in the interface.
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development
	  1.00   Fall'00  Shujing Jia			         Java  Modifications

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
class ChooserListModel	implements ListModel
{
	private Vector list = new Vector();
	private Vector listeners = new Vector();
	public ChooserListModel() {}

   /*********************************************************************************
    NAME:	addElement
	PURPOSE:
			  To add element to the model container - vector
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development
	  1.00   Fall'00  Shujing Jia			         Java  Modifications

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
	public void addElement(Object element)
	{
		Enumeration enumer = list.elements();
		int index = 0;
		final String elementString = element.toString();

		final int endOfList = list.size();
		list.addElement(element);
		notifyListeners(new ListDataEvent(this, ListDataEvent.INTERVAL_ADDED, endOfList, endOfList));

	}

   /*********************************************************************************
    NAME:		addListDataListener
	PURPOSE:
				To add list data listner
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
	public void addListDataListener(ListDataListener ldl)
	{
		listeners.addElement(ldl);
	}

   /*********************************************************************************
    NAME:		getElementAt
	PURPOSE:
				To retrieve the data element at specified index of vector
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
	public Object getElementAt(int index)
	{
		return list.elementAt(index);
	}

   /*********************************************************************************
    NAME:		getSize
	PURPOSE:
				To retireve size of vector
				getter/accessor
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
	public int getSize() { return list.size(); }

   /*********************************************************************************
    NAME:		insertElementAt
	PURPOSE:
				To insert element at an index i
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
	public void insertElementAt(Object obj,int index)
	{
		list.insertElementAt(obj,index);
		notifyListeners( new ListDataEvent(this, ListDataEvent.INTERVAL_ADDED,	index, index));
	}

   /*********************************************************************************
    NAME:		notifyListeners
	PURPOSE:
				To notify listners - this is for event operations.
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
	private void notifyListeners(ListDataEvent lde)
	{
		Enumeration enumer = listeners.elements();
		while (enumer.hasMoreElements())
		{
			ListDataListener ldl = (ListDataListener)enumer.nextElement();
			switch (lde.getType())
			{
			case ListDataEvent.CONTENTS_CHANGED:
				ldl.contentsChanged(lde);
				break;
			case ListDataEvent.INTERVAL_ADDED:
				ldl.intervalAdded(lde);
				break;
			case ListDataEvent.INTERVAL_REMOVED:
				ldl.intervalRemoved(lde);
				break;
			}
		}
	}

   /*********************************************************************************
    NAME:		removeElementAt
	PURPOSE:
				To remove an element at index i
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
	public void removeElementAt(int index)
	{
		list.removeElementAt(index);
		notifyListeners( new ListDataEvent(this, ListDataEvent.INTERVAL_REMOVED, index,index));
	}

   /*********************************************************************************
    NAME:		removeListDataListener
	PURPOSE:
				To remove list data listners
	RETURN VALUE:
	PROGRAM HISTORY
	Version  Date     Programmer                     Code  Reason
	-------  ----     ---------------------------    ----  --------------------
                      Gregg T. Stubbendieck, Ph.D.	 Java  Original development

	COMPUTER HARDWARE AND/OR SOFTWARE LIMITATIONS:

	PROJECT:	ModisTool
	NOTES:

    *********************************************************************************/
	public void removeListDataListener(ListDataListener ldl)
	{
		listeners.removeElement(ldl);
	}

}
