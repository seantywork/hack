

gh pr list

gh pr edit <num> --add-reviewer seantywork

gh pr edit <num> --add-assignee seantywork

gh pr merge <num> --merge

gh pr view <num> --json state,author,assignees,reviews,reviewRequests,mergeable

```json
{
  "author": {
    "id": "U_kgDOCNTH0g",
    "is_bot": false,
    "login": "hungry-spectre",
    "name": "egui"
  },
  "assignees":[
    {
      "id": "U_kgDOBmpJmg",
      "login": "seantywork",
      "name": "seantywork"
    }
  ],
  "reviewRequests": [
    {
      "__typename": "User",
      "login": "seantywork"
    }
  ],
  "reviews": [
    {
      "id": "PRR_kwDOKmvhuc5lhMqF",
      "author": {
        "login": "seantywork"
      },
      "authorAssociation": "OWNER",
      "body": "good",
      "submittedAt": "2023-10-30T03:37:46Z",
      "includesCreatedEdit": false,
      "reactionGroups": [],
      "state": "COMMENTED",
      "commit": {
        "oid": "f6e5a5b81b9af0c0dcbc9b3e11c4cacc4080e2ad"
      }
    },
    {
      "id": "PRR_kwDOKmvhuc5lhNKk",
      "author": {
        "login": "seantywork"
      },
      "authorAssociation": "OWNER",
      "body": "foff",
      "submittedAt": "2023-10-30T03:39:54Z",
      "includesCreatedEdit": false,
      "reactionGroups": [],
      "state": "CHANGES_REQUESTED",
      "commit": {
        "oid": "f6e5a5b81b9af0c0dcbc9b3e11c4cacc4080e2ad"
      }
    },
    {
      "id": "PRR_kwDOKmvhuc5lhNZk",
      "author": {
        "login": "seantywork"
      },
      "authorAssociation": "OWNER",
      "body": "",
      "submittedAt": "2023-10-30T03:41:47Z",
      "includesCreatedEdit": false,
      "reactionGroups": [],
      "state": "APPROVED",
      "commit": {
        "oid": "f6e5a5b81b9af0c0dcbc9b3e11c4cacc4080e2ad"
      }
    }
  ],
  "mergeable": "CONFLICTING",
  "state": "OPEN"
}

```


